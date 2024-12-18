using System.Runtime.InteropServices;
using UnityEngine;
using TMPro;

public class ZenohUser : MonoBehaviour
{
    const string DllName = "ZenohPubSubd";

    // ネイティブプラグインの関数を宣言
    [DllImport(DllName)]
    private static extern void StartPubSub(string pubKeyexpr, string subKeyexpr, CallbackDelegate callback);
    
    [DllImport(DllName)]
    private static extern void Publish(string message);

    [DllImport(DllName)]
    private static extern void Destroy();

    // コールバック関数のデリゲート
    private delegate void CallbackDelegate(string message);

    private static string receivedMessage;

    // 受信した文字列を表示するテキスト
    public TextMeshProUGUI text;

    private static readonly string keyexprPublish = "demo/example/zenoh-cpp-zenoh-c-pub";
    private static readonly string keyexprSubscribe = "demo/example/**";

    void Start() {
        StartPubSub(keyexprPublish, keyexprSubscribe, OnMessageReceived);
        Debug.Log("Callback set");

    }

    public void SendMessageToPlugin() {
        string msg = "Hello from Unity! : " + Time.frameCount;
        Publish(msg);
        Debug.Log(msg);
    }

    // コールバック関数
    [AOT.MonoPInvokeCallback(typeof(CallbackDelegate))]
    private static void OnMessageReceived(string message) {
        receivedMessage = message;
        Debug.Log("Received: " + message);
    }

    void Update()
    {
        SendMessageToPlugin();

        // receivedMessage = ReceiveString(); 
        if (receivedMessage != null)
        {
            text.text = receivedMessage;
        }
    }

    private void OnApplicationQuit()
    {
        Destroy();
    }
}
