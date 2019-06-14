<template>
  <el-card :header="$t('Authorization Required')" class="login">
    <el-form ref="login" :model="form" label-width="100px" label-position="left" :rules="rules">
      <el-form-item :label="$t('Username')" prop="username">
        <el-input v-model="form.username" prefix-icon="el-icon-user-solid" :placeholder="$t('Please input username')" @keyup.enter.native="handleLogin"></el-input>
      </el-form-item>
      <el-form-item :label="$t('Password')" prop="password">
        <el-input v-model="form.password" show-password prefix-icon="el-icon-lock" :placeholder="$t('Please input password')" @keyup.enter.native="handleLogin"></el-input>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="handleLogin" style="width: 70%">{{ $t('Login') }}</el-button>
        <el-button type="warning" @click="reset">{{ $t('Reset') }}</el-button>
      </el-form-item>
    </el-form>
  </el-card>
</template>

<script>
export default {
  data() {
    return {
      form: {
        username: '',
        password: ''
      },
      rules: {
        username: [
          {required: true, message: this.$t('This field is required')}
        ]
      }
    }
  },
  methods: {
    handleLogin() {
      this.$refs['login'].validate(valid => {
        if (valid) {
          this.$session.login(this.form.username, this.form.password).then(valid => {
            if (valid) {
              this.$router.push('/');
              return;
            }

            this.$message.error('Login fail');
          });
        }
      });
    },
    reset() {
      this.$refs['login'].resetFields();
    }
  },
  created() {
    this.$session.logout();
    this.$getLang();
  }
}
</script>

<style scoped>
.login {
  width: 500px;
  top: 50%;
  left: 50%;
  position: absolute;
  transform: translate(-50%, -50%);
}
</style>
