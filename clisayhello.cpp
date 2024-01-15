//디버스 클라이언트
#include <gio/gio.h>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    GError *error = NULL;
    GDBusConnection *connection;
    GVariant *result;
    gchar *response;

    /* D-Bus 시스템 버스에 연결 */
    connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (connection == NULL) {
        std::cerr << "Failed to connect to the D-Bus daemon: " << error->message << std::endl;
        g_error_free(error);
        return -1;
    }

    /* SayHello 메소드 호출 */
    result = g_dbus_connection_call_sync(connection,
                                         "org.example.HelloWorldService", // 서비스 이름
                                         "/org/example/HelloWorldObject", // 객체 경로
                                         "org.example.HelloWorld",        // 인터페이스 이름
                                         "SayHello",                      // 메소드 이름
                                         g_variant_new("(s)", "World"),   // 파라미터
                                         G_VARIANT_TYPE("(s)"),           // 응답 타입
                                         G_DBUS_CALL_FLAGS_NONE,
                                         -1,                              // 타임아웃 (기본)
                                         NULL,                            // 취소 가능 객체
                                         &error);
    if (result == NULL) {
        std::cerr << "Error calling method: " << error->message << std::endl;
        g_error_free(error);
        g_object_unref(connection);
        return -1;
    }

    /* 결과 출력 */
    g_variant_get(result, "(s)", &response);
    std::cout << "Received response: " << response << std::endl;

    g_free(response);
    g_variant_unref(result);
    g_object_unref(connection);

    return 0;
}
