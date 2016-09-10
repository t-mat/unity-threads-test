using UnityEngine;
using System.Runtime.InteropServices;

public class Test : MonoBehaviour {
	[DllImport("Win32Threads", EntryPoint = "TestFunc")]
	public static extern void TestFunc();

	void Start() {
		TestFunc();
	}

	void Update() {
	//	Debug.Log("");
	}
}
