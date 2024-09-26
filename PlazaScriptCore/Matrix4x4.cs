using System;
using System.Numerics;

namespace Plaza
{
    public class Matrix4
    {
        public float[,] data = new float[4, 4];

        public Matrix4()
        {

        }

        public Matrix4(float[,] newData)
        {
            data = newData;
        }

        public float this[int row, int col]
        {
            get => data[row, col];
            set => data[row, col] = value;
        }

        public void From1DArray(float[] source)
        {
            if (source.Length != 16)
                throw new ArgumentException("Source array must have 16 elements.");

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    data[i, j] = source[i * 4 + j];
                }
            }
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

        public static Vector4 TransformVector(Matrix4 matrix, Vector4 vector)
        {
            Vector4 result = matrix * vector;

            if (Math.Abs(result.W) > float.Epsilon)
            {
                result /= result.W;
            }

            return result;
        }

        public static Vector4 TransformVector(Vector4 vector, Matrix4 matrix)
        {
            Vector4 result = matrix * vector;

            if (Math.Abs(result.W) > float.Epsilon)
            {
                result /= result.W;
            }

            return result;
        }

        public float Determinant()
        {
            // Compute the determinant using cofactor expansion along the first row
            float a = data[0, 0];
            float b = data[0, 1];
            float c = data[0, 2];
            float d = data[0, 3];
            return a * Cofactor(0, 0) - b * Cofactor(0, 1) + c * Cofactor(0, 2) - d * Cofactor(0, 3);
        }

        private float Cofactor(int row, int col)
        {
            // Compute cofactor as Minor(row, col) * (-1)^(row+col)
            return Minor(row, col) * (((row + col) % 2 == 0) ? 1 : -1);
        }

        private float Minor(int row, int col)
        {
            // Create the 3x3 minor matrix by excluding the row and column
            float[,] minorMatrix = new float[3, 3];
            int minorRow = 0;

            for (int i = 0; i < 4; i++)
            {
                if (i == row) continue; // Skip the current row
                int minorCol = 0;

                for (int j = 0; j < 4; j++)
                {
                    if (j == col) continue; // Skip the current column
                    minorMatrix[minorRow, minorCol] = data[i, j];
                    minorCol++;
                }
                minorRow++;
            }

            // Return determinant of the 3x3 matrix
            return Determinant3x3(minorMatrix);
        }

        private float Determinant3x3(float[,] matrix)
        {
            // Calculate determinant of a 3x3 matrix
            return matrix[0, 0] * (matrix[1, 1] * matrix[2, 2] - matrix[1, 2] * matrix[2, 1]) -
                   matrix[0, 1] * (matrix[1, 0] * matrix[2, 2] - matrix[1, 2] * matrix[2, 0]) +
                   matrix[0, 2] * (matrix[1, 0] * matrix[2, 1] - matrix[1, 1] * matrix[2, 0]);
        }

        public static Matrix4 Inverse(Matrix4 matrix)
        {
            float det = matrix.Determinant();

            // Handle precision with an appropriate small value
            if (Math.Abs(det) < 1E-06f)
            {
                throw new InvalidOperationException("Matrix is not invertible. Determinant is too close to zero.");
            }

            // Calculate inverse by using cofactors, transposing, and dividing by determinant
            Matrix4 inverseMatrix = new Matrix4();

            // Loop through each element to calculate cofactor and set in transposed position
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    // Transpose: notice that j and i are swapped to transpose the cofactor matrix
                    inverseMatrix[j, i] = matrix.Cofactor(i, j) / det;
                }
            }

            return inverseMatrix;
        }
    }
}
