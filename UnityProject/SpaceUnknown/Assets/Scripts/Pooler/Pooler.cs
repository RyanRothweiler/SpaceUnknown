using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class Pooler
{
	public class Pool
	{
		public List<GameObject> available = new List<GameObject>();
		public GameObject fab;
		public Transform defaultParent;
		public string id;
	};

	private static Dictionary<string, Pool> pools = new Dictionary<string, Pool>();

	public static void Setup(string id, GameObject fab, int initCount, Transform defaultParent)
	{
		pools[id] = new Pool();
		pools[id].id = id;
		pools[id].defaultParent = defaultParent;
		pools[id].fab = fab;

		for (int i = 0; i < initCount; i++) {
			GameObject obj = CreateObj(pools[id]);
			pools[id].available.Add(obj);
		}
	}

	public static void Return(GameObject obj)
	{
		PoolTag tag = obj.GetComponent<PoolTag>();
		Debug.Assert(pools.ContainsKey(tag.poolID));

		Pool pool = pools[tag.poolID];
		obj.SetActive(false);

		if (!pool.available.Contains(obj)) {
			pool.available.Add(obj);
		}
	}

	public static GameObject Get(string id)
	{
		Debug.Assert(pools.ContainsKey(id));
		Pool pool = pools[id];

		GameObject obj = null;
		if (pool.available.Count >= 1) {
			obj = pool.available[pool.available.Count - 1];
			pool.available.RemoveAt(pool.available.Count - 1);
		} else {
			obj = CreateObj(pool);
		}

		obj.SetActive(true);
		return obj;
	}

	private static GameObject CreateObj(Pool pool)
	{
		GameObject obj = GameObject.Instantiate(pool.fab, pool.defaultParent);
		PoolTag tag = obj.AddComponent<PoolTag>();
		tag.poolID = pool.id;

		obj.SetActive(false);
		return obj;
	}
}