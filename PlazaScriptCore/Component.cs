using System;
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
        public UInt64 Uuid;

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
            return InternalCalls.HasComponent(Uuid, componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;
            T component = new T() { Entity = this };
            component.Uuid = this.Uuid;
            return component;
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
        public Vector3 rotation
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
        public Vector3 scale;

        public void MoveTowards(Vector3 vector3)
        {
            Plaza.InternalCalls.MoveTowards(this.Uuid, vector3);
        }
    }

    public class MeshRenderer : Component
    {
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
        /*
        public Vector2[] Uvs
        {
            get
            {
                InternalCalls.MeshRenderer_GetUvs(Entity.Uuid, out Vector2[] uvs);
                return uvs;
            }
            set
            {
                InternalCalls.MeshRenderer_SetUvs(Uuid, ref value);
            }
        }

        */
    }

    #region Rigid Body
    public struct Angular
    {
        public bool X, Y, Z;
    }
    public class RigidBody : Component
    {
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

    #region Script Component
    public class ScriptComponent : Component
    { 
        
    }

    #endregion Script Component

}
