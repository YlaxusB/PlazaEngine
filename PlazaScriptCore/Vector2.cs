using System;

namespace Plaza
{
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

        public static Vector2 operator *(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X * b.X, a.Y * b.Y);
        }

        public static Vector2 operator *(Vector2 a, float b)
        {
            return new Vector2(a.X * b, a.Y * b);
        }

        public static Vector2 operator /(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X / b.X, a.Y / b.Y);
        }

        public static float squareRoot(float number, double epsilon = 1e-6)
        {
            return (float)Math.Sqrt(number);
        }

        public static float Magnitude(Vector2 v)
        {
            return squareRoot(v.X * v.X + v.Y * v.Y);
        }

        public Vector2 Normalize()
        {
            float mag = Magnitude(this);
            if (mag == 0)
                mag = float.Epsilon;
            Vector2 result;
            result.X = this.X / mag;
            result.Y = this.Y / mag;
            return result;
        }

        public static float Dot(Vector2 a, Vector2 b)
        {
            return a.X * b.X + a.Y * b.Y;
        }

        public float LengthSquared()
        {
            return X * X + Y * Y;
        }

        public static float Distance(Vector2 v1, Vector2 v2)
        {
            return Magnitude(v1 - v2);
        }

        public static Vector2 Lerp(Vector2 v1, Vector2 v2, float time)
        {
            return v1 + (v2 - v1) * time;
        }
    }
}
