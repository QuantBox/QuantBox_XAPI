/*
 * This file was downloaded from the following location 
 * http://www.codeproject.com/KB/dotnet/enum.aspx
 * 
 * Author: Boris Dongarov (ideafixxxer)
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Ideafixxxer.Generics
{
    #region Enum

    /// <summary>
    /// Helper class for enum types
    /// </summary>
    /// <typeparam name="T">Must be enum type (declared using <c>enum</c> keyword)</typeparam>
    public static class Enum<T> where T : struct, IConvertible
    {
        private static readonly EnumConverter Converter;

        #region Nested types

        abstract class EnumConverter
        {
            public abstract string ToStringInternal(int value);
            public abstract int ParseInternal(string value, bool ignoreCase, bool parseNumber);
            public abstract bool TryParseInternal(string value, bool ignoreCase, bool parseNumber, out int result);
        }

        class ArrayEnumConverter : EnumConverter
        {
            private readonly string[] _names = Enum.GetNames(typeof(T));

            public ArrayEnumConverter(string[] names)
            {
                _names = names;
            }

            public override string ToStringInternal(int value)
            {
                return value >= 0 && value < _names.Length ? _names[value] : value.ToString();
            }

            public override int ParseInternal(string value, bool ignoreCase, bool parseNumber)
            {
                if (value == null) throw new ArgumentNullException("value");
                if (value.Length == 0) throw new ArgumentException("Value is empty", "value");
                char f = value[0];
                if (parseNumber && (Char.IsDigit(f) || f == '+' || f == '-'))
                    return Int32.Parse(value);
                StringComparison stringComparison = ignoreCase ? StringComparison.OrdinalIgnoreCase : StringComparison.Ordinal;
                for (int i = 0; i < _names.Length; i++)
                    if (_names[i].Equals(value, stringComparison))
                        return i;
                throw new ArgumentException("Enum value wasn't found", "value");
            }

            public override bool TryParseInternal(string value, bool ignoreCase, bool parseNumber, out int result)
            {
                result = 0;
                if (String.IsNullOrEmpty(value)) return false;
                char f = value[0];
                if (parseNumber && (Char.IsDigit(f) || f == '+' || f == '-'))
                {
                    int i;
                    if (Int32.TryParse(value, out i))
                    {
                        result = i;
                        return true;
                    }
                    return false;
                }
                StringComparison stringComparison = ignoreCase ? StringComparison.OrdinalIgnoreCase : StringComparison.Ordinal;
                for (int i = 0; i < _names.Length; i++)
                    if (_names[i].Equals(value, stringComparison))
                    {
                        result = i;
                        return true;
                    }
                return false;
            }
        }

        class DictionaryEnumConverter : EnumConverter
        {
            protected readonly Dictionary<int, string> _dic;

            public DictionaryEnumConverter(string[] names, T[] values)
            {
                _dic = new Dictionary<int, string>(names.Length);
                for (int j = 0; j < names.Length; j++)
                    _dic.Add(Convert.ToInt32(values[j], null), names[j]);
            }

            public override string ToStringInternal(int value)
            {
                string n;
                return _dic.TryGetValue(value, out n) ? n : value.ToString();
            }

            public override int ParseInternal(string value, bool ignoreCase, bool parseNumber)
            {
                if (value == null) throw new ArgumentNullException("value");
                if (value.Length == 0) throw new ArgumentException("Value is empty", "value");
                char f = value[0];
                if (parseNumber && (Char.IsDigit(f) || f == '+' || f == '-'))
                    return Int32.Parse(value);
                StringComparison stringComparison = ignoreCase ? StringComparison.OrdinalIgnoreCase : StringComparison.Ordinal;
                foreach (KeyValuePair<int, string> pair in _dic)
                {
                    if (pair.Value.Equals(value, stringComparison))
                        return pair.Key;
                }
                throw new ArgumentException("Enum value wasn't found", "value");
            }

            public override bool TryParseInternal(string value, bool ignoreCase, bool parseNumber, out int result)
            {
                result = 0;
                if (String.IsNullOrEmpty(value)) return false;
                char f = value[0];
                if (parseNumber && (Char.IsDigit(f) || f == '+' || f == '-'))
                {
                    int i;
                    if (Int32.TryParse(value, out i))
                    {
                        result = i;
                        return true;
                    }
                    return false;
                }
                StringComparison stringComparison = ignoreCase ? StringComparison.OrdinalIgnoreCase : StringComparison.Ordinal;
                foreach (KeyValuePair<int, string> pair in _dic)
                {
                    if (pair.Value.Equals(value, stringComparison))
                    {
                        result = pair.Key;
                        return true;
                    }
                }
                return false;
            }
        }

        class FlagsEnumConverter : DictionaryEnumConverter
        {
            private readonly uint[] _values;
            private static readonly string[] Seps = new[] { "," };

            public FlagsEnumConverter(string[] names, T[] values)
                : base(names, values)
            {
                _values = new uint[values.Length];
                for (int i = 0; i < values.Length; i++)
                    _values[i] = values[i].ToUInt32(null);
            }

            public override string ToStringInternal(int value)
            {
                string n;
                if (_dic.TryGetValue(value, out n)) return n;
                var sb = new StringBuilder();
                const string sep = ", ";
                uint uval;
                unchecked
                {
                    uval = (uint)value;

                    for (int i = _values.Length - 1; i >= 0; i--)
                    {
                        uint v = _values[i];
                        if (v == 0) continue;
                        if ((v & uval) == v)
                        {
                            uval &= ~v;
                            sb.Insert(0, sep).Insert(0, _dic[(int)v]);
                        }
                    }
                }
                return uval == 0 && sb.Length > sep.Length ? sb.ToString(0, sb.Length - sep.Length) : value.ToString();
            }

            public override int ParseInternal(string value, bool ignoreCase, bool parseNumber)
            {
                string[] parts = value.Split(Seps, StringSplitOptions.RemoveEmptyEntries);
                if (parts.Length == 1) return base.ParseInternal(value, ignoreCase, parseNumber);
                int val = 0;
                for (int i = 0; i < parts.Length; i++)
                {
                    string part = parts[i];
                    int t = base.ParseInternal(part.Trim(), ignoreCase, parseNumber);
                    val |= t;
                }
                return val;
            }

            public override bool TryParseInternal(string value, bool ignoreCase, bool parseNumber, out int result)
            {
                string[] parts = value.Split(Seps, StringSplitOptions.RemoveEmptyEntries);
                if (parts.Length == 1) return base.TryParseInternal(value, ignoreCase, parseNumber, out result);
                int val = 0;
                for (int i = 0; i < parts.Length; i++)
                {
                    string part = parts[i];
                    int t;
                    if (!base.TryParseInternal(part.Trim(), ignoreCase, parseNumber, out t))
                    {
                        result = 0;
                        return false;
                    }
                    val |= t;
                }
                result = val;
                return true;
            }
        }

        #endregion

        static Enum()
        {
            Type type = typeof(T);
            if (!type.IsEnum) 
                throw new ArgumentException("Generic Enum type works only with enums");
            string[] names = Enum.GetNames(type);
            var values = (T[])Enum.GetValues(type);
            if (type.GetCustomAttributes(typeof(FlagsAttribute), false).Length > 0)
            {
                Converter = new FlagsEnumConverter(names, values);
            }
            else
            {
                if (values.Where((t, i) => Convert.ToInt32(t) != i).Any())
                {
                    Converter = new DictionaryEnumConverter(names, values);
                }
                if (Converter == null)
                    Converter = new ArrayEnumConverter(names);
            }
        }

        /// <summary>
        /// Converts enum value to string
        /// </summary>
        /// <param name="value">Enum value converted to int</param>
        /// <returns>If <paramref name="value"/> is defined, the enum member name; otherwise the string representation of the <paramref name="value"/>.
        /// If <see cref="FlagsAttribute"/> is applied, can return comma-separated list of values</returns>
        public static string ToString(int value)
        {
            return Converter.ToStringInternal(value);
        }

        /// <summary>
        /// Converts enum value to string
        /// </summary>
        /// <param name="value">Enum value</param>
        /// <returns>If <paramref name="value"/> is defined, the enum member name; otherwise the string representation of the <paramref name="value"/>.
        /// If <see cref="FlagsAttribute"/> is applied, can return comma-separated list of values</returns>
        public static string ToString(T value)
        {
            return Converter.ToStringInternal(value.ToInt32(null));
        }

        public static T Parse(string value, bool ignoreCase = false, bool parseNumeric = true)
        {
            return (T) Enum.ToObject(typeof(T), Converter.ParseInternal(value, ignoreCase, parseNumeric));
        }

        public static bool TryParse(string value, bool ignoreCase, bool parseNumeric, out T result)
        {
            int ir;
            bool b = Converter.TryParseInternal(value, ignoreCase, parseNumeric, out ir);
            result = (T) Enum.ToObject(typeof(T), ir);
            return b;
        }

        public static bool TryParse(string value, bool ignoreCase, out T result)
        {
            int ir;
            bool b = Converter.TryParseInternal(value, ignoreCase, true, out ir);
            result = (T)Enum.ToObject(typeof(T), ir);
            return b;
        }

        public static bool TryParse(string value, out T result)
        {
            int ir;
            bool b = Converter.TryParseInternal(value, false, true, out ir);
            result = (T)Enum.ToObject(typeof(T), ir);
            return b;
        }
    }

    #endregion
}
