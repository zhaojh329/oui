<template>
  <div>
    <a-spin size="large" class="oui-login" :spinning="spinning">
      <a-card :title="$t('login.Authorization Required')">
        <a-form-model :model="form">
          <a-form-model-item prop="username">
            <a-input v-model="form.username" @pressEnter="handleLogin" :placeholder="$t('login.Please input username')">
              <a-icon slot="prefix" type="user"/>
            </a-input>
          </a-form-model-item>
          <a-form-model-item prop="password">
            <a-input-password v-model="form.password" @pressEnter="handleLogin" type="password" :placeholder="$t('login.Please input password')">
              <a-icon slot="prefix" type="lock"/>
            </a-input-password>
          </a-form-model-item>
          <a-form-model-item>
            <a-button type="primary" size="large" block @click="handleLogin">{{ $t("login.Login") }}</a-button>
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
      form: {
        username: '',
        password: ''
      },
      spinning: false
    }
  },
  methods: {
    handleLogin () {
      this.spinning = true
      this.$session.login(this.form.username, this.form.password).then((ok) => {
        if (ok) {
          this.$router.push('/')
          return
        }
        this.$message.error(this.$t('login.Wrong password given!'), 1)
        this.spinning = false
      })
    }
  },
  created () {
    this.$session.logout()
  }
}
</script>

<style>
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
