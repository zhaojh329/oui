<template>
  <Card class="login" title="Background management system">
    <Form ref="formValidate" :model="formValidate" :rules="ruleValidate">
      <Form-item prop="username">
        <Input v-model="formValidate.username" size="large" prefix="md-person" placeholder="Please input username" />
      </Form-item>
      <Form-item prop="password">
        <Input v-model="formValidate.password" type="password" size="large" prefix="md-lock" placeholder="Please input password" />
      </Form-item>
      <Form-item>
        <Button type="primary" size="large" long @click="handleLogin">Login</Button>
      </Form-item>
    </Form>
  </Card>
</template>

<script>
export default {
  data () {
    return {
      formValidate: {},
      ruleValidate: {
        username: [{required: true, message: 'Username can not be empty'}]
      }
    }
  },
  methods: {
    handleLogin () {
      this.$refs.formValidate.validate(valid => {
        if (valid) {
          let routes = [
            {
              path: '/status',
              component: () => import('@/views/Main.vue'),
              children: [
                {
                  path: 'syslog',
                  component: () => import('@/views/SysLog.vue')
                }
              ]
            }
          ];

          let menus = [
            {
              name: 'status',
              title: 'Status',
              children: [
                {
                  name: 'SysLog',
                  title: 'SysLog',
                  path: '/status/syslog'
                }
              ]
            }
          ];

          this.$store.commit('setMenus', menus);
          this.$router.addRoutes(routes);
          this.$router.push('/');
        }
      });
    }
  }
}
</script>

<style>
.login {
  width: 400px;
  left: 50%;
  margin-left: -200px;
  height: 250px;
  top: 50%;
  margin-top: -125px;
}
</style>
