using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Plaza
{
    public static class InternalCalls
    {

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static ulong FindEntityByNameCall(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool HasComponent(UInt64 uuid, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool InputIsKeyDown(KeyCode keyCode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool InputIsMouseDown(int button);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetMouseDelta(out Vector2 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void MoveTowards(UInt64 uuid, Vector3 vector3);

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

        #endregion Transform

        #region MeshRenderer

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
        #endregion MeshRenderer
    }
}
