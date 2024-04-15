using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using static Plaza.Physics;

namespace Plaza
{
    public enum Axis
    {
        X,
        Y,
        Z
    }
    public static class InternalCalls
    {

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static ulong FindEntityByNameCall(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static UInt64 NewEntity();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool HasComponent(UInt64 uuid, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AddComponent(UInt64 uuid, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RemoveComponent(UInt64 uuid, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool HasScript(UInt64 uuid, Type scriptType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static object GetScript(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AddScript(UInt64 uuid, Type scriptType);

        #region Input

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool InputIsKeyDown(KeyCode keyCode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool IsKeyReleased(KeyCode keyCode);



        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool InputIsMouseDown(int button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetMouseDelta(out Vector2 vector);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CursorHide(bool val);
        #endregion Input

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MoveTowards(UInt64 uuid, Vector3 vector3);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MoveTowardsReturn(UInt64 uuid, Vector3 vector3, out Vector3 outVector);




        #region Entity;
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static string EntityGetName(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void EntitySetName(UInt64 uuid, string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static UInt64 EntityGetParent(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static List<UInt64> EntityGetChildren(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void EntitySetParent(UInt64 uuid, UInt64 parentUuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void EntityDelete(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static UInt64 Instantiate(UInt64 uuid);
        #endregion Entity;

        #region Physics
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Physics_Raycast(Vector3 origin, Vector3 direction, float maxDistance, out RaycastHit hit, UInt64 ignoredUuid = 0);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Physics_RaycastSpecific(Vector3 origin, Vector3 direction, float maxDistance, out RaycastHit hit, UInt64 specificUuid);
        #endregion Physics
        #region Components

        #region Transform

        public static Vector3 GetPosition(UInt64 uuid)
        {
            GetPositionCall(uuid, out Vector3 vector);
            return vector;
        }

        public static Vector3 GetRotation(UInt64 uuid)
        {
            GetRotationCall(uuid, out Vector3 vector);
            return vector;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetPosition(UInt64 uuid, ref Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetPositionCall(UInt64 uuid, out Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetRotation(UInt64 uuid, ref Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetRotationCall(UInt64 uuid, out Vector3 vector);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetRotationQuaternion(UInt64 uuid, ref Vector4 quaternion);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetRotationQuaternionCall(UInt64 uuid, out Vector4 quaternion);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetWorldRotationQuaternionCall(UInt64 uuid, out Vector4 quaternion);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetScaleCall(UInt64 uuid, ref Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetScaleCall(UInt64 uuid, out Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Transform_GetForwardVector(UInt64 uuid, out Vector3 vec);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Transform_GetUpVector(UInt64 uuid, out Vector3 vec);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Transform_GetLeftVector(UInt64 uuid, out Vector3 vec);

        [StructLayout(LayoutKind.Sequential)]
        public struct FloatArray
        {
            public float Value;
        }
        public static float[] Transform_GetWorldMatrix(UInt64 uuid)
        {
            int size = 0;
            IntPtr ptr = IntPtr.Zero;
            Transform_GetWorldMatrix(uuid, ref ptr, ref size);

            if (size > 0 && ptr != IntPtr.Zero)
            {
                int floatArrayLength = size;
                float[] result = new float[floatArrayLength];

                for (int i = 0; i < floatArrayLength; i++)
                {
                    FloatArray floatArray = (FloatArray)Marshal.PtrToStructure(ptr + i * Marshal.SizeOf(typeof(FloatArray)), typeof(FloatArray));
                    result[i] = floatArray.Value;
                }
                return result;
            }
            else
            {
                return null; // Handle empty or null data
            }
        }

        // Import the C++ function with the modified signature
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Transform_GetWorldMatrix(UInt64 uuid, ref IntPtr ptr, ref int size);



        #endregion Transform

        #region MeshRenderer
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MeshRenderer_SetMaterial(UInt64 uuid, UInt64 materialUuid);
        public static Vector3[] MeshRenderer_GetVertices(UInt64 uuid)
        {
            int size = 0;
            IntPtr ptr = IntPtr.Zero;
            MeshRenderer_GetVertices(uuid, ref ptr, ref size);

            if (size > 0 && ptr != IntPtr.Zero)
            {
                Vector3[] result = new Vector3[size];
                for (int i = 0; i < size; i++)
                {
                    result[i] = (Vector3)Marshal.PtrToStructure(ptr + i * Marshal.SizeOf<Vector3>(), typeof(Vector3));
                }
                return result;
            }
            else
            {
                return null; // Handle empty or null data
            }
        }

        // Import the C++ function with the modified signature
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_GetVertices(UInt64 uuid, ref IntPtr ptr, ref int size);


        public static void MeshRenderer_SetVertices(UInt64 uuid, Vector3[] vertices)
        {
            int size = vertices.Length;
            IntPtr ptr = Marshal.AllocHGlobal(size * Marshal.SizeOf<Vector3>());
            for (int i = 0; i < size; i++)
            {
                Marshal.StructureToPtr(vertices[i], ptr + i * Marshal.SizeOf<Vector3>(), false);
            }

            MeshRenderer_SetVertices(uuid, ptr, size);

            Marshal.FreeHGlobal(ptr);
        }

        // Import the C++ function with the modified signature
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_SetVertices(UInt64 uuid, IntPtr vertices, int size);




        public static int[] MeshRenderer_GetIndices(UInt64 uuid)
        {
            int size = 0;
            IntPtr ptr = IntPtr.Zero;
            MeshRenderer_GetIndices(uuid, ref ptr, ref size);

            if (size > 0 && ptr != IntPtr.Zero)
            {
                int[] result = new int[size];
                for (int i = 0; i < size; i++)
                {
                    result[i] = (int)Marshal.PtrToStructure(ptr + i * Marshal.SizeOf<int>(), typeof(int));
                }
                return result;
            }
            else
            {
                return null; // Handle empty or null data
            }
        }

        // Import the C++ function with the modified signature
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_GetIndices(UInt64 uuid, ref IntPtr ptr, ref int size);


        public static void MeshRenderer_SetIndices(UInt64 uuid, int[] indices)
        {
            int size = indices.Length;
            IntPtr ptr = Marshal.AllocHGlobal(size * Marshal.SizeOf<int>());
            for (int i = 0; i < size; i++)
            {
                Marshal.StructureToPtr(indices[i], ptr + i * Marshal.SizeOf<int>(), false);
            }

            MeshRenderer_SetIndices(uuid, ptr, size);

            Marshal.FreeHGlobal(ptr);
        }

        // Import the C++ function with the modified signature
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_SetIndices(UInt64 uuid, IntPtr indices, int size);

        /* Normals */

        public static Vector3[] MeshRenderer_GetNormals(UInt64 uuid)
        {
            int size = 0;
            IntPtr ptr = IntPtr.Zero;
            MeshRenderer_GetNormals(uuid, ref ptr, ref size);

            if (size > 0 && ptr != IntPtr.Zero)
            {
                Vector3[] result = new Vector3[size];
                for (int i = 0; i < size; i++)
                {
                    result[i] = (Vector3)Marshal.PtrToStructure(ptr + i * Marshal.SizeOf<Vector3>(), typeof(Vector3));
                }
                return result;
            }
            else
            {
                return null; // Handle empty or null data
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_GetNormals(UInt64 uuid, ref IntPtr ptr, ref int size);

        public static void MeshRenderer_SetNormals(UInt64 uuid, Vector3[] normals)
        {
            int size = normals.Length;
            IntPtr ptr = Marshal.AllocHGlobal(size * Marshal.SizeOf<Vector3>());
            for (int i = 0; i < size; i++)
            {
                Marshal.StructureToPtr(normals[i], ptr + i * Marshal.SizeOf<Vector3>(), false);
            }

            MeshRenderer_SetNormals(uuid, ptr, size);

            Marshal.FreeHGlobal(ptr);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_SetNormals(UInt64 uuid, IntPtr indices, int size);

        /* Uvs */
        public static Vector2[] MeshRenderer_GetUvs(UInt64 uuid)
        {
            int size = 0;
            IntPtr ptr = IntPtr.Zero;
            MeshRenderer_GetUvs(uuid, ref ptr, ref size);

            if (size > 0 && ptr != IntPtr.Zero)
            {
                Vector2[] result = new Vector2[size];
                for (int i = 0; i < size; i++)
                {
                    result[i] = (Vector2)Marshal.PtrToStructure(ptr + i * Marshal.SizeOf<Vector2>(), typeof(Vector2));
                }
                return result;
            }
            else
            {
                return null; // Handle empty or null data
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_GetUvs(UInt64 uuid, ref IntPtr ptr, ref int size);

        public static void MeshRenderer_SetUvs(UInt64 uuid, Vector2[] normals)
        {
            int size = normals.Length;
            IntPtr ptr = Marshal.AllocHGlobal(size * Marshal.SizeOf<Vector2>());
            for (int i = 0; i < size; i++)
            {
                Marshal.StructureToPtr(normals[i], ptr + i * Marshal.SizeOf<Vector2>(), false);
            }

            MeshRenderer_SetUvs(uuid, ptr, size);

            Marshal.FreeHGlobal(ptr);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_SetUvs(UInt64 uuid, IntPtr indices, int size);

        static void MarshalMesh()
        {

        }
        public static void MeshRenderer_SetMesh(UInt64 uuid, Vector3[] vertices, int[] indices, Vector3[] normals, Vector2[] uvs)
        {
            int verticesSize = vertices.Length;
            int indicesSize = indices.Length;
            int normalsSize = normals.Length;
            int uvsSize = uvs.Length;

            IntPtr verticesPtr = Marshal.AllocHGlobal(verticesSize * Marshal.SizeOf<Vector3>());
            for (int i = 0; i < verticesSize; i++)
            {
                Marshal.StructureToPtr(vertices[i], verticesPtr + i * Marshal.SizeOf<Vector3>(), false);
            }

            IntPtr indicesPtr = Marshal.AllocHGlobal(indicesSize * Marshal.SizeOf<int>());
            for (int i = 0; i < indicesSize; i++)
            {
                Marshal.StructureToPtr(indices[i], indicesPtr + i * Marshal.SizeOf<int>(), false);
            }

            IntPtr normalsPtr = Marshal.AllocHGlobal(normalsSize * Marshal.SizeOf<Vector3>());
            for (int i = 0; i < normalsSize; i++)
            {
                Marshal.StructureToPtr(normals[i], normalsPtr + i * Marshal.SizeOf<Vector3>(), false);
            }

            IntPtr uvsPtr = Marshal.AllocHGlobal(uvsSize * Marshal.SizeOf<Vector2>());
            for (int i = 0; i < uvsSize; i++)
            {
                Marshal.StructureToPtr(uvs[i], uvsPtr + i * Marshal.SizeOf<Vector2>(), false);
            }

            MeshRenderer_SetMesh(uuid, verticesPtr, verticesSize, indicesPtr, indicesSize, normalsPtr, normalsSize, uvsPtr, uvsSize);

            Marshal.FreeHGlobal(verticesPtr);
            Marshal.FreeHGlobal(indicesPtr);
            Marshal.FreeHGlobal(normalsPtr);
            Marshal.FreeHGlobal(uvsPtr);
        }

        // Import the C++ function with the modified signature
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private extern static void MeshRenderer_SetMesh(UInt64 uuid, IntPtr vertices, int verticesSize, IntPtr indices, int indicesSize, IntPtr normals, int normalsSize, IntPtr uvs, int uvsSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float MeshRenderer_GetHeight(UInt64 uuid, float pixelX, float pixelY);

        #endregion MeshRenderer

        #region RigidBody
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_ApplyForce(UInt64 uuid, ref Vector3 force);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_AddForce(UInt64 uuid, ref Vector3 force, ForceMode mode, bool autowake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_AddTorque(UInt64 uuid, ref Vector3 torque, ForceMode mode, bool autowake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float RigidBody_GetDrag(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_SetDrag(UInt64 uuid, float drag);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float RigidBody_GetVelocity(UInt64 uuid, out Vector3 velocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_SetVelocity(UInt64 uuid, ref Vector3 velocity);



        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_LockAngular(UInt64 uuid, Axis axis, bool value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static Angular RigidBody_IsAngularLocked(UInt64 uuid);
        #endregion RigidBody

        #region Collider
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Collider_AddShape(UInt64 uuid, ColliderShapeEnum shape);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Collider_AddShapeMeshCall(UInt64 uuid, ColliderShapeEnum shape, IntPtr vertices, int verticesSize, IntPtr indices, int indicesSize, IntPtr normals, int normalsSize, IntPtr uvs, int uvsSize);
        public static void Collider_AddShapeMesh(UInt64 uuid, ColliderShapeEnum shape, Mesh mesh) {
            int verticesSize = mesh.Vertices.Length;
            int indicesSize = mesh.Indices.Length;
            int normalsSize = mesh.Normals.Length;
            int uvsSize = mesh.Uvs.Length;

            IntPtr verticesPtr = Marshal.AllocHGlobal(verticesSize * Marshal.SizeOf<Vector3>());
            for (int i = 0; i < verticesSize; i++)
            {
                Marshal.StructureToPtr(mesh.Vertices[i], verticesPtr + i * Marshal.SizeOf<Vector3>(), false);
            }

            IntPtr indicesPtr = Marshal.AllocHGlobal(indicesSize * Marshal.SizeOf<int>());
            for (int i = 0; i < indicesSize; i++)
            {
                Marshal.StructureToPtr(mesh.Indices[i], indicesPtr + i * Marshal.SizeOf<int>(), false);
            }

            IntPtr normalsPtr = Marshal.AllocHGlobal(normalsSize * Marshal.SizeOf<Vector3>());
            for (int i = 0; i < normalsSize; i++)
            {
                Marshal.StructureToPtr(mesh.Normals[i], normalsPtr + i * Marshal.SizeOf<Vector3>(), false);
            }

            IntPtr uvsPtr = Marshal.AllocHGlobal(uvsSize * Marshal.SizeOf<Vector2>());
            for (int i = 0; i < uvsSize; i++)
            {
                Marshal.StructureToPtr(mesh.Uvs[i], uvsPtr + i * Marshal.SizeOf<Vector2>(), false);
            }

            //MeshRenderer_SetMesh(uuid, verticesPtr, verticesSize, indicesPtr, indicesSize, normalsPtr, normalsSize, uvsPtr, uvsSize);

            Collider_AddShapeMeshCall(uuid, shape, verticesPtr, verticesSize, indicesPtr, indicesSize, normalsPtr, normalsSize, uvsPtr, uvsSize);
            Marshal.FreeHGlobal(verticesPtr);
            Marshal.FreeHGlobal(indicesPtr);
            Marshal.FreeHGlobal(normalsPtr);
            Marshal.FreeHGlobal(uvsPtr);
        }
        /*
                 public UInt64 uuid;
        public Vector3[] Vertices;

        public int[] Indices;

        public Vector3[] Normals;
        public Vector2[] Uvs;
         */

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Collider_AddShapeHeightFieldCall(UInt64 uuid, ColliderShapeEnum shape, float[] heightData, int size);
        static float[] FlattenArray(float[,] array)
        {
            int rows = array.GetLength(0);
            int cols = array.GetLength(1);

            float[] flattenedArray = new float[rows * cols];

            for (int i = 0; i < rows; i++)
            {
                for (int j = 0; j < cols; j++)
                {
                    flattenedArray[i * cols + j] = array[i, j];
                }
            }

            return flattenedArray;
        }
        public static void Collider_AddShapeHeightField(UInt64 uuid, ColliderShapeEnum shape, float[,] array, int size)
        {
            float[] flattenArray = FlattenArray(array);
            InternalCalls.Collider_AddShapeHeightFieldCall(uuid, shape, flattenArray, size);
        }

        #endregion Collider

        #region CharacterController
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void CharacterController_MoveCall(UInt64 uuid, Vector3 position, float minimumDistance, bool followOrientation, float elapsedTime);
        #endregion CharacterController

        #region TextRenderer
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static string TextRenderer_GetText(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TextRenderer_SetText(UInt64 uuid, string text);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TextRenderer_GetPosition(UInt64 uuid, out Vector2 position);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TextRenderer_SetPosition(UInt64 uuid, ref Vector2 position);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float TextRenderer_GetScale(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TextRenderer_SetScale(UInt64 uuid, float position);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TextRenderer_SetFullText(UInt64 uuid, string text, float x, float y, float scale, ref Vector4 color);
        #endregion TextRenderer

        #region AudioSource
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioSource_Play(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioSource_Stop(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AudioSource_GetSpatial(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioSource_SetSpatial(UInt64 uuid, bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float AudioSource_GetVolume(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioSource_SetVolume(UInt64 uuid, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float AudioSource_GetPitch(UInt64 uuid);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AudioSource_SetPitch(UInt64 uuid, float value);
        #endregion AudioSource

        #endregion Components

        #region Time
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float Time_GetDeltaTime();
        #endregion Time
    }
}
