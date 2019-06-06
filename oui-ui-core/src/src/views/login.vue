<template>
  <el-card header="Authorization Required" class="login">
    <el-form ref="login" :model="form" label-width="100px" label-position="left">
      <el-form-item label="Username" prop="username" required>
        <el-input v-model="form.username" prefix-icon="el-icon-user-solid" placeholder="Please input username" @keyup.enter.native="handleLogin"></el-input>
      </el-form-item>
      <el-form-item label="Password" prop="password">
        <el-input v-model="form.password" type="password" prefix-icon="el-icon-lock" placeholder="Please input password" @keyup.enter.native="handleLogin"></el-input>
      </el-form-item>
      <el-form-item>
        <el-button type="primary" @click="handleLogin" style="width: 70%">Login</el-button>
        <el-button type="warning" @click="reset">Reset</el-button>
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
  mounted() {
    this.$session.logout();
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
