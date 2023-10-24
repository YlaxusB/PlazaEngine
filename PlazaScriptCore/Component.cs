using System;
using System.CodeDom;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Plaza;

namespace Plaza
{
    public class Entity
    {

        public UInt64 Uuid;
        public string Name
        {
            get
            {
                return InternalCalls.EntityGetName(this.Uuid);
            }
            set {
                InternalCalls.EntitySetName(this.Uuid, value);
            }
        }

        public Entity parent
        {
            get
            {
                return new Entity(InternalCalls.EntityGetParent(this.Uuid));
            }
            set
            {
                InternalCalls.EntitySetParent(this.Uuid, value.Uuid);
            }
        }

        public List<Entity> Children
        {
            get
            {
                List<UInt64> childrenUuid = InternalCalls.EntityGetChildren(Uuid);
                List<Entity> childrenList = new List<Entity>();
                foreach (UInt64 child in childrenUuid)
                {
                    childrenList.Add(new Entity(child));
                }
                return childrenList;
            }
        }

        public Entity()
        {

        }

        public Entity(ulong uuid)
        {
            Uuid = uuid;
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            if (componentType.IsSubclassOf(typeof(ScriptComponent)))
            {
                return InternalCalls.HasScript(Uuid, componentType);
            }
            return InternalCalls.HasComponent(Uuid, componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;
            if (typeof(T).IsSubclassOf(typeof(ScriptComponent)))
            {
                T component = new T() { Entity = this };
                component.Uuid = this.Uuid;
                return component;
            }
            else
            {
                T component = new T() { Entity = this };
                component.Uuid = this.Uuid;
                return component;
            }
        }

        public T AddComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
                return null;
            T component = new T() { Entity = this };
            component.Uuid = this.Uuid;
            InternalCalls.AddComponent(this.Uuid, typeof(T));
            return component;
        }

        public void RemoveComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return;
            InternalCalls.RemoveComponent(this.Uuid, typeof(T));
        }

        public T GetScript<T>() where T : Entity, new()
        {
            return InternalCalls.GetScript(this.Uuid) as T;
        }

        public Entity FindEntityByName(string name)
        {
            ulong entityUuid = InternalCalls.FindEntityByNameCall(name);
            if (entityUuid == 0)
                return null;
            return new Entity(entityUuid);
        }

        public Entity Instantiate(Entity otherEntity)
        {
            return new Entity(InternalCalls.Instantiate(otherEntity.Uuid));
        }

        public void Delete()
        {
            InternalCalls.EntityDelete(this.Uuid);
        }
    }

    public abstract class Component : Entity
    {
        public UInt64 Uuid;
        public Entity Entity { get; internal set; }
    }



    public class Transform : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.GetPositionCall(Entity.Uuid, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.SetPosition(Uuid, ref value);
            }
        }
        public Vector3 Rotation
        {
            get
            {
                InternalCalls.GetRotationCall(Entity.Uuid, out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.SetRotation(Uuid, ref value);
            }
        }
        public Vector3 Scale
        {
            get
            {
                InternalCalls.GetScaleCall(Entity.Uuid, out Vector3 rotation);
                return rotation;
            }
            set
            {
                InternalCalls.SetScaleCall(Uuid, ref value);
            }
        }

        public Vector3 ForwardVector
        {
            get
            {
                InternalCalls.Transform_GetForwardVector(Entity.Uuid, out Vector3 vec);
                return vec;
            }
        }

        public Vector3 UpVector
        {
            get
            {
                InternalCalls.Transform_GetUpVector(Entity.Uuid, out Vector3 vec);
                return vec;
            }
        }

        public Vector3 LeftVector
        {
            get
            {
                InternalCalls.Transform_GetLeftVector(Entity.Uuid, out Vector3 vec);
                return vec;
            }
        }

        public void MoveTowards(Vector3 vector3)
        {
            Plaza.InternalCalls.MoveTowards(this.Uuid, vector3);
        }
    }

    public class Mesh
    {
        public Mesh()
        {

        }
        public UInt64 uuid;
        public Vector3[] Vertices;

        public int[] Indices;

        public Vector3[] Normals;
        public Vector2[] Uvs;
    }

    public class MeshRenderer : Component
    {
        public Mesh mesh
        {
            set
            {
                InternalCalls.MeshRenderer_SetVertices(Uuid, value.Vertices);
                InternalCalls.MeshRenderer_SetIndices(Uuid, value.Indices);
                InternalCalls.MeshRenderer_SetNormals(Uuid, value.Normals);
                InternalCalls.MeshRenderer_SetUvs(Uuid, value.Uvs);
            }
        }
        public Vector3[] Vertices
        {
            get
            {
                return InternalCalls.MeshRenderer_GetVertices(Entity.Uuid);
            }
            set
            {
                InternalCalls.MeshRenderer_SetVertices(Uuid, value);
            }
        }

        public int[] Indices
        {
            get
            {
                return InternalCalls.MeshRenderer_GetIndices(Entity.Uuid);
            }
            set
            {
                InternalCalls.MeshRenderer_SetIndices(Uuid, value);
            }
        }

        public Vector3[] Normals
        {
            get
            {
                return InternalCalls.MeshRenderer_GetNormals(Entity.Uuid);
            }
            set
            {
                InternalCalls.MeshRenderer_SetNormals(Uuid, value);
            }
        }
        
        public Vector2[] Uvs
        {
            get
            {
                return InternalCalls.MeshRenderer_GetUvs(Entity.Uuid);
            }
            set
            {
                InternalCalls.MeshRenderer_SetUvs(Uuid, value);
            }
        }
    }

    #region Rigid Body
    public struct Angular
    {
        public bool X, Y, Z;
    }
    public enum ForceMode
    {
        FORCE,
        IMPULSE,
        VELOCITY_CHANGE,
        ACCELERATION
    }

    public class RigidBody : Component
    {
        public void ApplyForce(Vector3 force)
        {
            InternalCalls.RigidBody_ApplyForce(Uuid, ref force);
        }
        public void AddForce(Vector3 force, ForceMode mode = ForceMode.FORCE, bool autowake = true)
        {
            InternalCalls.RigidBody_AddForce(Uuid, ref force, mode, autowake);
        }
        public void AddTorque(Vector3 torque, ForceMode mode = ForceMode.FORCE, bool autowake = true)
        {
            InternalCalls.RigidBody_AddTorque(Uuid, ref torque, mode, autowake);
        }
        public void LockAngular(Axis axis, bool value)
        {
            InternalCalls.RigidBody_LockAngular(this.Uuid, axis, value);
        }

        public Angular IsAngularLocked()
        {
            return InternalCalls.RigidBody_IsAngularLocked(this.Uuid);
        }
    }
    #endregion

    #region Collider
    public enum ColliderShapeEnum
    {
        BOX,
        SPHERE,
        CAPSULE,
        PLANE,
        CYLINDER,
        MESH,
        CONVEX_MESH
    };
    public class Collider : Component
    {
        public void AddShape(ColliderShapeEnum shape)
        {
            InternalCalls.Collider_AddShape(this.Uuid, shape);
        }
    }
    #endregion Collider

    #region TextRenderer
    public class TextRenderer : Component
    {
        public string text
        {
            get
            {
                return InternalCalls.TextRenderer_GetText(this.Uuid);
            }
            set
            {
                InternalCalls.TextRenderer_SetText(this.Uuid, value);
            }
        }
        public Vector2 position
        {
            get
            {
                InternalCalls.TextRenderer_GetPosition(Uuid, out Vector2 position);
                return position;
            }
            set
            {
                InternalCalls.TextRenderer_SetPosition(Uuid, ref value);
            }
        }

        public float scale
        {
            get
            {
                return InternalCalls.TextRenderer_GetScale(Entity.Uuid);
            }
            set
            {
                InternalCalls.TextRenderer_SetScale(Uuid, value);
            }
        }
        public void SetText(string text = "", float x = 0, float y = 0, float scale = 1.0f, Vector4 color = default(Vector4))
        {
            InternalCalls.TextRenderer_SetFullText(Uuid, text, x, y, scale, ref color);
        }
    }
    #endregion TextRenderer

    #region Script Component
    public class ScriptComponent : Component
    {

    }

    public class Script : ScriptComponent { }


    #endregion Script Component

}
