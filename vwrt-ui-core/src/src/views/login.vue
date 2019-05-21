<template>
  <Card class="login" title="Authorization Required">
    <Form ref="formValidate" :model="formValidate" :rules="ruleValidate">
      <FormItem prop="username">
        <Input v-model="formValidate.username" size="large" prefix="md-person" placeholder="Please input username" @on-enter="handleLogin"/>
      </FormItem>
      <FormItem prop="password">
        <Input v-model="formValidate.password" type="password" size="large" prefix="md-lock" placeholder="Please input password" @on-enter="handleLogin"/>
      </FormItem>
      <FormItem>
        <Button type="primary" size="large" long @click="handleLogin">Login</Button>
      </FormItem>
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
          this.$session.login(this.formValidate.username, this.formValidate.password).then(valid => {
            if (valid) {
              this.$router.push('/');
              return;
            }

            this.$Message.error('Login fail');
          });
        }
      });
    }
  },
  mounted() {
    this.$session.logout();
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
