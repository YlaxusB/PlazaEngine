using System;

namespace Plaza
{
    public class Quaternion
    {
        public float x, y, z, w;

        public Quaternion() { }
        public Quaternion(float X, float Y, float Z, float W)
        {
            this.x = X;
            this.y = Y;
            this.z = Z;
            this.w = W;
        }

        public Quaternion(Vector3 vector)
        {
            this.x = vector.X;
            this.y = vector.Y;
            this.z = vector.Z;
            this.w = CalculateW();
        }

        private float CalculateW()
        {
            return (float)Math.Sqrt(1.0 - x * x - y * y - z * z);
        }

        public static Quaternion Euler(float x, float y, float z)
        {
            double cy = Math.Cos(x * 0.5);
            double sy = Math.Sin(x * 0.5);
            double cp = Math.Cos(y * 0.5);
            double sp = Math.Sin(y * 0.5);
            double cr = Math.Cos(z * 0.5);
            double sr = Math.Sin(z * 0.5);

            Quaternion q = new Quaternion();
            q.w = (float)(cr * cp * cy + sr * sp * sy);
            q.x = (float)(sr * cp * cy - cr * sp * sy);
            q.y = (float)(cr * sp * cy + sr * cp * sy);
            q.z = (float)(cr * cp * sy - sr * sp * cy);

            return q;

        }

        public static Quaternion operator +(Quaternion a, Quaternion b)
        {
            return new Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        public static Quaternion operator -(Quaternion a, Quaternion b)
        {
            return new Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }

        public static Quaternion operator *(Quaternion a, Quaternion b)
        {
            Quaternion result = new Quaternion();

            result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
            result.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
            result.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
            result.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;

            // Normalize the result to ensure it represents a valid rotation
            result.Normalize();

            return result;
        }

        public static Vector3 operator *(Quaternion rotation, Vector3 point)
        {
            float num = rotation.x * 2f;
            float num2 = rotation.y * 2f;
            float num3 = rotation.z * 2f;
            float num4 = rotation.x * num;
            float num5 = rotation.y * num2;
            float num6 = rotation.z * num3;
            float num7 = rotation.x * num2;
            float num8 = rotation.x * num3;
            float num9 = rotation.y * num3;
            float num10 = rotation.w * num;
            float num11 = rotation.w * num2;
            float num12 = rotation.w * num3;
            Vector3 result = default(Vector3);
            result.X = (1f - (num5 + num6)) * point.X + (num7 - num12) * point.Y + (num8 + num11) * point.Z;
            result.Y = (num7 + num12) * point.X + (1f - (num4 + num6)) * point.Y + (num9 - num10) * point.Z;
            result.Z = (num8 - num11) * point.X + (num9 + num10) * point.Y + (1f - (num4 + num5)) * point.Z;
            return result;
        }

        public void Normalize()
        {
            float magnitude = (float)Math.Sqrt(x * x + y * y + z * z + w * w);
            if (magnitude > 0)
            {
                float invMagnitude = 1.0f / magnitude;
                x *= invMagnitude;
                y *= invMagnitude;
                z *= invMagnitude;
                w *= invMagnitude;
            }
            else
            {
                // Handle the case when the quaternion has zero magnitude
                // Set to identity or take another appropriate action.
                // For now, set to identity.
                this.x = 0;
                this.y = 0;
                this.z = 0;
                this.w = 1;
            }
        }

        public Quaternion ToDegrees()
        {
            return new Quaternion(this.x * Mathf.Rad2Deg, this.y * Mathf.Rad2Deg, this.z * Mathf.Rad2Deg, this.w);
        }

        public Quaternion ToRadians()
        {
            return new Quaternion(this.x * Mathf.Deg2Rad, this.y * Mathf.Deg2Rad, this.z * Mathf.Deg2Rad, this.w);
        }

        // Other Quaternion operations (multiplication, normalization, etc.) can be added here.
    }
}
