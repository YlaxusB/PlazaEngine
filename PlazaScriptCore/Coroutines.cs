using System;
using System.Collections.Generic;
using System.Collections;

namespace Plaza
{
    public class Coroutine
    {
        private IEnumerator _routine;

        public Coroutine(IEnumerator routine)
        {
            _routine = routine;
        }

        public bool MoveNext()
        {
            return _routine.MoveNext();
        }

        static IEnumerator WaitForSeconds(float seconds)
        {
            float startTime = DateTime.Now.Second;
            while (DateTime.Now.Second - startTime < seconds)
            {
                yield return null;
            }
        }
    }

    public class CoroutineManager
    {
        private List<Coroutine> _coroutines = new List<Coroutine>();

        public Coroutine StartCoroutine(IEnumerator routine)
        {
            Coroutine coroutine = new Coroutine(routine);
            _coroutines.Add(coroutine);
            return coroutine;
        }

        public void StopCoroutine(Coroutine coroutine)
        {
            _coroutines.Remove(coroutine);  // Remove the coroutine from the active list
        }

        public void Update()
        {
            for (int i = _coroutines.Count - 1; i >= 0; i--)
            {
                if (!_coroutines[i].MoveNext())
                {
                    _coroutines.RemoveAt(i);
                }
            }
        }
    }
}
