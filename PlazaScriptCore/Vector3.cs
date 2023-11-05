using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

        public static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta)
        {
            Vector3 direction = target - current;
            float distance = Magnitude(direction);
            return maxDistanceDelta >= distance ? target : current + Normalize(direction) * maxDistanceDelta;
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

        public static Vector4 operator /(Vector4 vector, float value)
        {
            return new Vector4(vector.X / value, vector.Y / value, vector.Z / value, vector.W / value);
        }
    }

}
