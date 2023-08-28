using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using Plaza;

namespace Plaza
{
    public class Entity
    {
        UInt64 uuid;
    }

    public class Component
    {
        UInt64 uuid;
        Entity entity;
    }

    public class Transform : Component 
    {

    }
}
