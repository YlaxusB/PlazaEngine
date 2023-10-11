using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Plaza
{

    public struct Vector3
    {
        public float X, Y, Z;
        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
        public Vector3(float x)
        {
            X = x;
            Y = x;
            Z = x;
        }

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        public static Vector3 operator -(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }

        public static Vector3 operator *(Vector3 a, float b)
        {
            return new Vector3(a.X * b, a.Y * b, a.Z * b);
        }

        public static Vector3 Cross(Vector3 v1, Vector3 v2)
        {
            Vector3 result = new Vector3();
            result.X = (v1.Y * v2.Z) - (v1.Z * v2.Y);
            result.Y = (v1.Z * v2.X) - (v1.X * v2.Z);
            result.Z = (v1.X * v2.Y) - (v1.Y * v2.X);
            return result;
        }

        public static float squareRoot(float number, double epsilon = 1e-6)
        {
            return (float)Math.Sqrt(number);
        }

        public static float Magnitude(Vector3 v)
        {
            return squareRoot(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
        }

        public static Vector3 Normalize(Vector3 v)
        {
            float mag = Magnitude(v);
            if (mag == 0)
                mag = float.Epsilon;
            Vector3 result;
            result.X = v.X / mag;
            result.Y = v.Y / mag;
            result.Z = v.Z / mag;
            return result;
        }

        public static float Distance(Vector3 v1, Vector3 v2)
        {
            return Magnitude(v1 - v2);
        }
    }

    public struct Vector2
    {
        public float X, Y;
        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public Vector2(float x)
        {
            X = x;
            Y = x;
        }

        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X + b.X, a.Y + b.Y);
        }

        public static Vector2 operator -(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X - b.X, a.Y - b.Y);
        }
    }

    public struct Vector4
    {
        public float X, Y, Z, W;
        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }
        public Vector4(float x)
        {
            X = x;
            Y = x;
            Z = x;
            W = x;
        }
    }

}
