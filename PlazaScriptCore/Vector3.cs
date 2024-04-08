using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using System.Security.Cryptography.X509Certificates;

namespace Plaza
{
    public class Matrix4
    {
        public float[,] data;

        public Matrix4()
        {
            data = new float[4, 4];
        }

        public Matrix4(float[,] newData)
        {
            data = newData;
        }

        // Accessor for individual elements
        public float this[int row, int col]
        {
            get => data[row, col];
            set => data[row, col] = value;
        }

        public Vector3 ExtractRotationAsEulerAngles()
        {
            var m = new Matrix4x4(
                data[0, 0], data[0, 1], data[0, 2], 0,
                data[1, 0], data[1, 1], data[1, 2], 0,
                data[2, 0], data[2, 1], data[2, 2], 0,
                0, 0, 0, 1
            ); // Convert the float[,] data to Matrix4x4

            System.Numerics.Quaternion quaternionRotation = System.Numerics.Quaternion.CreateFromRotationMatrix(m);
            Vector3 eulerRotation = QuaternionToEulerAngles(quaternionRotation);

            return eulerRotation;
        }

        private Vector3 QuaternionToEulerAngles(System.Numerics.Quaternion q)
        {
            // Convert a Quaternion to Euler Angles
            Vector3 euler = new Vector3();

            double sinr_cosp = 2 * (q.W * q.X + q.Y * q.Z);
            double cosr_cosp = 1 - 2 * (q.X * q.X + q.Y * q.Y);
            euler.X = (float)Math.Atan2(sinr_cosp, cosr_cosp);

            double sinp = 2 * (q.W * q.Y - q.Z * q.X);
            if (Math.Abs(sinp) >= 1)
                euler.Y = (float)(sinp >= 0 ? Math.PI / 2 : -Math.PI / 2); // Use 90 degrees if out of range
            else
                euler.Y = (float)Math.Asin(sinp);

            double siny_cosp = 2 * (q.W * q.Z + q.X * q.Y);
            double cosy_cosp = 1 - 2 * (q.Y * q.Y + q.Z * q.Z);
            euler.Z = (float)Math.Atan2(siny_cosp, cosy_cosp);

            return euler;
        }

        // Identity matrix creation
        public static Matrix4 Identity()
        {
            Matrix4 result = new Matrix4();
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    result[i, j] = (i == j) ? 1.0f : 0.0f;
                }
            }
            return result;
        }

        public Matrix4x4 ToSystemNumericsMatrix4x4()
        {
            Matrix4x4 result = new Matrix4x4();

            for (int row = 0; row < 4; row++)
            {
                for (int col = 0; col < 4; col++)
                {
                    result.M11 = data[0, 0];
                    result.M12 = data[0, 1];
                    result.M13 = data[0, 2];
                    result.M14 = data[0, 3];

                    result.M21 = data[1, 0];
                    result.M22 = data[1, 1];
                    result.M23 = data[1, 2];
                    result.M24 = data[1, 3];

                    result.M31 = data[2, 0];
                    result.M32 = data[2, 1];
                    result.M33 = data[2, 2];
                    result.M34 = data[2, 3];

                    result.M41 = data[3, 0];
                    result.M42 = data[3, 1];
                    result.M43 = data[3, 2];
                    result.M44 = data[3, 3];
                }
            }

            return result;
        }
        public static Matrix4 FromSystemNumericsMatrix4x4(Matrix4x4 inputMatrix)
        {
            Matrix4 result = new Matrix4();

            result[0, 0] = inputMatrix.M11;
            result[0, 1] = inputMatrix.M12;
            result[0, 2] = inputMatrix.M13;
            result[0, 3] = inputMatrix.M14;

            result[1, 0] = inputMatrix.M21;
            result[1, 1] = inputMatrix.M22;
            result[1, 2] = inputMatrix.M23;
            result[1, 3] = inputMatrix.M24;

            result[2, 0] = inputMatrix.M31;
            result[2, 1] = inputMatrix.M32;
            result[2, 2] = inputMatrix.M33;
            result[2, 3] = inputMatrix.M34;

            result[3, 0] = inputMatrix.M41;
            result[3, 1] = inputMatrix.M42;
            result[3, 2] = inputMatrix.M43;
            result[3, 3] = inputMatrix.M44;

            return result;
        }

        public static Matrix4 operator *(Matrix4 matrix1, Matrix4 matrix2)
        {
            if (matrix1.data.GetLength(1) != matrix2.data.GetLength(0))
            {
                throw new InvalidOperationException("Matrix dimensions are not compatible for multiplication.");
            }

            int m = matrix1.data.GetLength(0); // Number of rows in the result matrix
            int n = matrix2.data.GetLength(1); // Number of columns in the result matrix
            int common = matrix1.data.GetLength(1); // Number of columns in matrix1, also the number of rows in matrix2

            // Create a new matrix to store the result
            float[,] resultData = new float[m, n];

            // Perform matrix multiplication
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    float sum = 0;
                    for (int k = 0; k < common; k++)
                    {
                        sum += matrix1.data[i, k] * matrix2.data[k, j];
                    }
                    resultData[i, j] = sum;
                }
            }

            return new Matrix4(resultData);
        }

        public static Vector3 operator *(Matrix4 matrix, Vector3 vector)
        {
            if (matrix.data.GetLength(0) != 4 || matrix.data.GetLength(1) != 4)
            {
                throw new InvalidOperationException("Matrix dimensions are not 4x4.");
            }

            float x = matrix.data[0, 0] * vector.X + matrix.data[0, 1] * vector.Y + matrix.data[0, 2] * vector.Z + matrix.data[0, 3];
            float y = matrix.data[1, 0] * vector.X + matrix.data[1, 1] * vector.Y + matrix.data[1, 2] * vector.Z + matrix.data[1, 3];
            float z = matrix.data[2, 0] * vector.X + matrix.data[2, 1] * vector.Y + matrix.data[2, 2] * vector.Z + matrix.data[2, 3];

            return new Vector3(x, y, z);
        }

        public static Vector4 operator *(Matrix4 matrix, Vector4 vector)
        {
            if (matrix.data.GetLength(0) != 4 || matrix.data.GetLength(1) != 4)
            {
                throw new InvalidOperationException("Matrix dimensions are not 4x4.");
            }

            float x = matrix.data[0, 0] * vector.X + matrix.data[0, 1] * vector.Y + matrix.data[0, 2] * vector.Z + matrix.data[0, 3] * vector.W;
            float y = matrix.data[1, 0] * vector.X + matrix.data[1, 1] * vector.Y + matrix.data[1, 2] * vector.Z + matrix.data[1, 3] * vector.W;
            float z = matrix.data[2, 0] * vector.X + matrix.data[2, 1] * vector.Y + matrix.data[2, 2] * vector.Z + matrix.data[2, 3] * vector.W;
            float w = matrix.data[3, 0] * vector.X + matrix.data[3, 1] * vector.Y + matrix.data[3, 2] * vector.Z + matrix.data[3, 3] * vector.W;

            return new Vector4(x, y, z, w);
        }

        public static Vector3 TransformVector(Matrix4 matrix, Vector3 vector)
        {
            // Assuming matrix is a 4x4 transformation matrix
            // Create a homogeneous vector (with w component set to 1 for a point)
            Vector4 homogeneousVec = new Vector4(vector.X, vector.Y, vector.Z, 1.0f);

            // Perform the transformation by multiplying the matrix and the homogeneous vector
            Vector4 transformedVec = matrix * homogeneousVec;

            // Divide by the w component to convert back to Cartesian coordinates
            if (Math.Abs(transformedVec.W) > float.Epsilon)
            {
                transformedVec /= transformedVec.W;
            }

            // Create a Vector3 from the transformed coordinates
            return new Vector3(transformedVec.X, transformedVec.Y, transformedVec.Z);
        }
    }
    public struct Vector3
    {
        public float X, Y, Z;
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

        public static Vector4 operator /(Vector4 vector, float value)
        {
            return new Vector4(vector.X / value, vector.Y / value, vector.Z / value, vector.W / value);
        }
    }

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
    }
}
