<template>
  <el-card class="login">
    <template #header>
      <div class="header">{{ $t('Login') }}</div>
    </template>
    <el-form ref="form" :model="formValue" :rules="rules" label-width="80px" label-suffix=":" size="large">
      <el-form-item :label="$t('Username')" prop="username">
        <el-input v-model="formValue.username" prefix-icon="user" :placeholder="$t('Please enter username')" @keyup.enter="login" autofocus/>
      </el-form-item>
      <el-form-item :label="$t('Password')" prop="password">
        <el-input type="password" v-model="formValue.password" prefix-icon="lock" :placeholder="$t('Please enter password')" @keyup.enter="login" show-password/>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" :loading="loading" @click="login" class="login-button">{{ $t('Login') }}</el-button>
      </el-form-item>
    </el-form>
    <el-divider/>
    <div class="copyright">
      <p>Copyright © 2022 Powered by <a href="https://github.com/zhaojh329/oui" target="_blank">oui</a></p>
    </div>
  </el-card>
</template>

<script>
export default {
  data() {
    return {
      loading: false,
      formValue: {
        username: '',
        password: ''
      },
      rules: {
        username: {
          required: true,
          trigger: 'blur',
          message: () => this.$t('Please enter username')
        }
      }
    }
  },
  methods: {
    login() {
      this.$refs.form.validate(async valid => {
        if (!valid)
          return

        this.loading = true

        try {
          await this.$oui.login(this.formValue.username, this.formValue.password)
          this.$router.push('/')
        } catch {
          this.$message.error(this.$t('wrong username or password'))
        }

        this.loading = false
      })
    }
  }
}
</script>

<style scoped>
.header {
  text-align: center;
}

.login {
  width: 500px;
  top: 40%;
  left: 50%;
  position: fixed;
  transform: translate(-50%, -50%);
}

.login-button {
  width: 100%;
}

.copyright {
  text-align: right;
  font-size: 1.2em;
  color: #888;
}
</style>

<i18n src="./locale.json"/>
