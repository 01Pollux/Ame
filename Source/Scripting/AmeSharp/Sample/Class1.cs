using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;

namespace Example.Managed
{
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class CustomAttribute : Attribute
    {
        public float Value;
    }
    public struct MyVec3
    {
        public float X;
        public float Y;
        public float Z;
    }

    [Custom(Value = -2500.0f)]
    public class ExampleClass
    {
        public int myPrivateValue;

        public ExampleClass(int someValue)
        {
            Console.WriteLine($"Example({someValue})");
        }

        public static int StaticMethod(float value)
        {
            Console.WriteLine($"StaticMethod: {value}");
            return (int)value - 26;
        }

        public static int SumArray(UnmanagedNativeArray<int> arr, string[] strs, UnmanagedNativeString str2, int[] arr2, int[][] arr3, string withDefault = "default", int x = 15)
        {
            return arr.ToArray().Concat(arr2).Concat(arr3.SelectMany(x => x)).ToArray().Sum();
        }

        public void MemberMethod(MyVec3 vec3)
        {
            MyVec3 anotherVector = new()
            {
                X = 10,
                Y = 20,
                Z = 30
            };

            vec3.X += anotherVector.X;
            vec3.Y += anotherVector.Y;
            vec3.Z += anotherVector.Z;
            Console.WriteLine($"X: {vec3.X}, Y: {vec3.Y}, Z: {vec3.Z}");
        }

        public void StringDemo()
        {
            string str = "Hello, World?";
            Console.WriteLine(str);
        }

        public int PublicProp
        {
            get => myPrivateValue;
            set => myPrivateValue = value * 2;
        }
    }
}