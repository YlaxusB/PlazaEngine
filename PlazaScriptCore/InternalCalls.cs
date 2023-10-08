using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

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
        public extern static bool HasComponent(UInt64 uuid, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void AddComponent(UInt64 uuid, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RemoveComponent(UInt64 uuid, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool HasScript(UInt64 uuid, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static object GetScript(UInt64 uuid);

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




        #region Entity;
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
        public extern static void SetScaleCall(UInt64 uuid, ref Vector3 vector);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void GetScaleCall(UInt64 uuid, out Vector3 vector);

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


        #region RigidBody
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_ApplyForce(UInt64 uuid, ref Vector3 force);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void RigidBody_LockAngular(UInt64 uuid, Axis axis, bool value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static Angular RigidBody_IsAngularLocked(UInt64 uuid);
        #endregion RigidBody

        #region Collider
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Collider_AddShape(UInt64 uuid, ColliderShapeEnum shape);

        #endregion Collider

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

        #endregion Components

        #region Time
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static float Time_GetDeltaTime();
        #endregion Time
    }
}
