using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Plaza
{
    public class Screen
    {
        public static Vector2 Size
        {
            get
            {
                InternalCalls.Screen_GetSize(out Vector2 vector);
                return vector;
            }
        }
    }
}
