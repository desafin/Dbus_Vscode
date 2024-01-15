//디버스 서버
#include <gio/gio.h>

/* DBus 서비스의 인트로스펙션 데이터입니다. 여기서 서비스의 구조를 정의합니다. */
static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='org.example.HelloWorld'>"
  "    <method name='SayHello'>"
  "      <arg type='s' name='greeting' direction='in'/>" /* 인사말을 입력으로 받습니다. */
  "      <arg type='s' name='response' direction='out'/>" /* 응답도 문자열로 보냅니다. */
  "    </method>"
  "  </interface>"
  "</node>";

/* 메소드 호출을 처리하는 함수입니다. 여기서 실제 작업을 모두 처리합니다. */
static void handle_method_call (GDBusConnection       *connection,
                                const gchar           *sender,
                                const gchar           *object_path,
                                const gchar           *interface_name,
                                const gchar           *method_name,
                                GVariant              *parameters,
                                GDBusMethodInvocation *invocation,
                                gpointer               user_data) {
  /* SayHello 메소드를 호출한 경우에만 반응합니다. */
  if (g_strcmp0 (method_name, "SayHello") == 0) {
    const gchar *greeting;
    g_variant_get (parameters, "(&s)", &greeting); /* 입력된 인사말을 가져옵니다. */
    gchar *response = g_strdup_printf ("DBus를 통해 호출되는 SayHello 메서드입니다 테스트 코드, %s!", greeting); /* 응답 문자열을 만듭니다. */
    g_dbus_method_invocation_return_value (invocation, g_variant_new ("(s)", response)); /* 응답을 보냅니다. */
    g_free (response); /* 메모리를 정리합니다. */
  }
}

/* 서비스의 인터페이스 vtable을 정의합니다. 여기서는 메소드 호출만 처리하므로 나머지는 NULL로 설정합니다. */
static const GDBusInterfaceVTable interface_vtable = {
  handle_method_call,
  NULL, /* handle_get_property */
  NULL  /* handle_set_property */
};

/* DBus에 서비스가 등록될 때 호출되는 함수입니다. 여기서 객체를 등록하고 본격적인 작업을 시작합니다. */
static void on_bus_acquired (GDBusConnection *connection,
                             const gchar     *name,
                             gpointer         user_data) {
  guint registration_id = g_dbus_connection_register_object (connection,
                                                             "/org/example/HelloWorldObject",
                                                             g_dbus_node_info_lookup_interface (g_dbus_node_info_new_for_xml (introspection_xml, NULL),
                                                                                                "org.example.HelloWorld"),
                                                             &interface_vtable,
                                                             NULL, /* user_data */
                                                             NULL, /* user_data_free_func */
                                                             NULL); /* GError** */
  /* 등록 ID가 유효한지(0보다 큰지) 확인하는 것이 좋습니다. */
}

/* 메인 함수입니다, 모든 것이 시작되는 곳입니다! */
int main (int argc, char *argv[]) {
  guint owner_id;
  GMainLoop *loop;

  /* DBus에 서비스 이름을 등록합니다. 이제부터 우리 서비스가 DBus에서 활동한다고 알립니다. */
  owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                             "org.example.HelloWorldService",
                             G_BUS_NAME_OWNER_FLAGS_NONE,
                             on_bus_acquired,
                             NULL, /* on_name_acquired */
                             NULL, /* on_name_lost */
                             NULL, /* user_data */
                             NULL); /* user_data_free_func */

  /* 메인 루프를 생성하고 실행합니다. 이 루프가 종료되면 프로그램도 종료됩니다. */
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  /* 더 이상 서비스를 호스팅하지 않으므로 등록을 해제합니다. */
  g_bus_unown_name (owner_id);

  return 0;
}