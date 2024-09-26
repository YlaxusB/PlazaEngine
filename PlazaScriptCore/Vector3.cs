using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using System.Security.Cryptography.X509Certificates;

namespace Plaza
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector2 XY
        {
            get
            {
                return new Vector2(X, Y);
            }
        }

        public Vector2 XZ
        {
            get
            {
                return new Vector2(X, Z);
            }
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3(double x, double y, double z)
        {
            X = (float)x;
            Y = (float)y;
            Z = (float)z;
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

        public static Vector3 operator *(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
        }

        public static Vector3 operator /(Vector3 a, float b)
        {
            return new Vector3(a.X / b, a.Y / b, a.Z / b);
        }

        private static float CopySign(float sizeval, float signval)
        {
            return Math.Abs(sizeval) * Math.Sign(signval);
        }

        public static Vector3 ToEulerAngles(Quaternion q)
        {
            Vector3 angles = new Vector3();

            // Roll (X-axis rotation)
            float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
            float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
            angles.X = (float)Math.Atan2(sinr_cosp, cosr_cosp);

            // Pitch (Y-axis rotation)
            float sinp = 2 * (q.w * q.y - q.z * q.x);
            if (Math.Abs(sinp) >= 1)
                angles.Y = (float)CopySign((float)Math.PI / 2, sinp); // Use 90 degrees if out of range
            else
                angles.Y = (float)Math.Asin(sinp);

            // Yaw (Z-axis rotation)
            float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
            float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
            angles.Z = (float)Math.Atan2(siny_cosp, cosy_cosp);

            return angles;
        }

        public static Vector3 Cross(Vector3 v1, Vector3 v2)
        {
            Vector3 result = new Vector3();
            result.X = (v1.Y * v2.Z) - (v1.Z * v2.Y);
            result.Y = (v1.Z * v2.X) - (v1.X * v2.Z);
            result.Z = (v1.X * v2.Y) - (v1.Y * v2.X);
            return result;
        }

        public static float Dot(Vector3 a, Vector3 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
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

        public Vector3 Normalize()
        {
            return Vector3.Normalize(this);
        }

        public static float Angle(Vector3 a, Vector3 b)
        {
            float dot = Dot(Normalize(a), Normalize(b));
            dot = Math.Min(1f, Math.Max(-1f, dot));
            float angleRad = (float)Math.Acos(dot);
            return angleRad * (180f / (float)Math.PI);
        }

        public static float Distance(Vector3 v1, Vector3 v2)
        {
            return Magnitude(v1 - v2);
        }

        public Vector3 Transform(Matrix4 transformationMatrix)
        {
            // Ensure the matrix is 4x4 to represent a transformation matrix
            if (transformationMatrix.data.GetLength(0) != 4 || transformationMatrix.data.GetLength(1) != 4)
            {
                throw new ArgumentException("Matrix should be a 4x4 transformation matrix");
            }

            float[] vector = new float[] { X, Y, Z, 1 }; // Extend to a 4x1 vector

            float[] transformedVector = new float[3];
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    transformedVector[i] += vector[j] * transformationMatrix[j, i];
                }
            }

            return new Vector3(transformedVector[0], transformedVector[1], transformedVector[2]);
        }

        public Vector3 InverseTransform(Matrix4 transformationMatrix, Vector3 worldCoordinate)
        {
            if (transformationMatrix.data.GetLength(0) != 4 || transformationMatrix.data.GetLength(1) != 4)
            {
                throw new ArgumentException("Matrix should be a 4x4 transformation matrix");
            }

            // Extend the vector to a 4x1 by adding a 1 as the fourth component (homogeneous coordinates)
            float[] vector = new float[] { worldCoordinate.X, worldCoordinate.Y, worldCoordinate.Z, 1 };

            // Create the inverse of the transformation matrix
            System.Numerics.Matrix4x4.Invert(transformationMatrix.ToSystemNumericsMatrix4x4(), out Matrix4x4 result);
            Matrix4 inverseMatrix = Matrix4.FromSystemNumericsMatrix4x4(result);
            // Perform the inverse transformation
            float[] transformedVector = new float[3];
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    transformedVector[i] += vector[j] * inverseMatrix[j, i];
                }
            }

            return new Vector3(transformedVector[0], transformedVector[1], transformedVector[2]);
        }

        public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta)
        {
            Vector3 direction = target - current;
            float distance = Magnitude(direction);
            return maxDistanceDelta >= distance ? target : current + Normalize(direction) * maxDistanceDelta;
        }

        public Vector3 Sin()
        {
            return new Vector3(Math.Sin(this.X), Math.Sin(this.Y), Math.Sin(this.Z));
        }
        public Vector3 Cos()
        {
            return new Vector3(Math.Cos(this.X), Math.Cos(this.Y), Math.Cos(this.Z));
        }

        public static Vector3 ProjectOnPlane(Vector3 vector, Vector3 planeNormal)
        {
            Vector3 normalizedPlaneNormal = Vector3.Normalize(planeNormal);
            float dot = Vector3.Dot(vector, normalizedPlaneNormal);
            return vector - new Vector3(dot, dot, dot) * normalizedPlaneNormal;
        }

        public Vector3 LookAt(Vector3 target)
        {
            return LookAt(target, new Vector3(0.0f, 1.0f, 0.0f));
        }

        public Vector3 LookAt(Vector3 target, Vector3 up)
        {
            Vector3 source = this;
            Vector3 forward = Vector3.Normalize(target - source);
            Vector3 right = Vector3.Normalize(Vector3.Cross(up, forward));
            Vector3 newUp = Vector3.Cross(forward, right);

            float[,] matrix = new float[3, 3] { { right.X, right.Y, right.Z }, { newUp.X, newUp.Y, newUp.Z }, { forward.X, forward.Y, forward.Z } };
            float det = matrix[0, 0] * matrix[1, 1] * matrix[2, 2] + matrix[0, 1] * matrix[1, 2] * matrix[2, 0] + matrix[0, 2] * matrix[1, 0] * matrix[2, 1]
                      - matrix[0, 2] * matrix[1, 1] * matrix[2, 0] - matrix[0, 1] * matrix[1, 0] * matrix[2, 2] - matrix[0, 0] * matrix[1, 2] * matrix[2, 1];

            if (det < 0)
            {
                right = right * -1.0f;
                matrix = new float[3, 3] { { right.X, right.Y, right.Z }, { newUp.X, newUp.Y, newUp.Z }, { forward.X, forward.Y, forward.Z } };
            }

            return new Vector3(matrix[0, 0], matrix[1, 0], matrix[2, 0]);
        }

        public static Vector3 Lerp(Vector3 v1, Vector3 v2, float time)
        {
            return v1 + (v2 - v1) * time;
        }
    }
}
