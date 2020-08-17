<template>
  <div>
    <a-spin size="large" class="oui-login" :spinning="spinning">
      <a-card :title="$t('Authorization Required')">
        <a-form-model ref="login" :model="form" :rules="rules" v-bind="layout">
          <a-form-model-item :label="$t('Username')" prop="username">
            <a-input v-model="form.username" @pressEnter="handleLogin" :placeholder="$t('Please input username')">
              <a-icon slot="prefix" type="user"/>
            </a-input>
          </a-form-model-item>
          <a-form-model-item :label="$t('Password')" prop="password">
            <a-input-password v-model="form.password" @pressEnter="handleLogin" type="password" :placeholder="$t('Please input password')">
              <a-icon slot="prefix" type="lock"/>
            </a-input-password>
          </a-form-model-item>
          <a-form-model-item :wrapper-col="{ span: 20, offset: 4 }">
            <a-button type="primary" size="large" @click="handleLogin" style="width: 60%;">{{ $t("Login") }}</a-button>
            <a-button type="warning" size="large" @click="reset" style="margin-left: 15px">{{ $t("Reset") }}</a-button>
          </a-form-model-item>
        </a-form-model>
      </a-card>
    </a-spin>
    <div class="footer">
      <a href="https://github.com/zhaojh329/oui" target="_blank">Powered by oui</a>
    </div>
  </div>
</template>

<script>
export default {
  data () {
    return {
      layout: {
        labelCol: { span: 4 },
        wrapperCol: { span: 20 }
      },
      form: {
        username: '',
        password: ''
      },
      rules: {
        username: [
          { required: true, message: () => this.$t('validator.required') }
        ]
      },
      spinning: false
    }
  },
  methods: {
    handleLogin () {
      this.$refs.login.validate((valid) => {
        if (valid) {
          this.spinning = true
          this.$session.login(this.form.username, this.form.password).then((ok) => {
            if (ok) {
              this.$session.updateACLs().then(() => {
                this.$router.push('/')
              })
              return
            }
            this.$message.error(this.$t('Wrong username or password given!'), 1)
            this.spinning = false
          })
        }
      })
    },
    reset () {
      this.$refs.login.resetFields()
    }
  },
  created () {
    this.$session.logout()
  }
}
</script>

<style lang="stylus">
.oui-login {
  width: 500px;
  top: 50%;
  left: 50%;
  position: fixed;
  transform: translate(-50%, -50%);
}

.footer {
  font-size: 20px;
  position: fixed;
  bottom: 10px;
  right: 20px;
  color: blue;
}
</style>
