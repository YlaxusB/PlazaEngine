using System;

namespace Plaza
{
    public static class Mathf
    {
        public const float Deg2Rad = (float)(Math.PI / 180.0);
        public const float Rad2Deg = (float)(180.0 / Math.PI);

        public static float Sin(float angle)
        {
            return (float)Math.Sin(angle);
        }

        public static float Cos(float angle)
        {
            return (float)Math.Cos(angle);
        }

        public static float Lerp(float a, float b, float time)
        {
            return a + (b - a) * time;
        }
    }
}
