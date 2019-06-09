<template>
  <el-tabs value="password">
    <el-tab-pane label="Router Password" name="password">
      <el-card title="Router Password" style="width: 500px">
        <el-form label-width="200px" :model="passwordForm" :rules="passwordRules" ref="passwordForm">
          <el-form-item label="Password" prop="password">
            <el-input type="password" v-model="passwordForm.password"></el-input>
          </el-form-item>
          <el-form-item label="Confirmation" prop="confirm">
            <el-input type="password" v-model="passwordForm.confirm"></el-input>
          </el-form-item>
          <el-form-item>
            <el-button type="primary" size="mini"  @click="setPassword" style="float: right;margin-right: 50px;">Save</el-button>
          </el-form-item>
        </el-form>
      </el-card>
    </el-tab-pane>
    <el-tab-pane label="SSH Access" name="dropbear">
      <uci-form config="dropbear">
        <uci-section title="SSH Server" type="dropbear">
          <uci-option type="list" label="Interface" name="Interface" :options="interfaces"></uci-option>
          <uci-option type="input" label="Port" name="Port" placeholder="22"></uci-option>
          <uci-option type="switch" label="Password authentication" name="PasswordAuth" initial="on" active-value="on" inactive-value="off"></uci-option>
          <uci-option type="switch" label="Allow root logins with password" name="RootPasswordAuth" initial="on" active-value="on" inactive-value="off"></uci-option>
          <uci-option type="switch" label="Gateway ports" name="GatewayPorts" active-value="on" inactive-value="off"></uci-option>
        </uci-section>
      </uci-form>
    </el-tab-pane>
  </el-tabs>
</template>

<script>

export default {
  name: 'admin',
  data() {
    const validatePass = (rule, value, callback) => {
      if (value === '') {
        callback(new Error('Please enter your password'));
      } else {
        if (this.passwordForm.confirm !== '')
          this.$refs['passwordForm'].validateField('confirm');
        callback();
      }
    };

    const validatorConfirm = (rule, value, callback) => {
      if (value === '') {
        callback(new Error('Please enter your password again'));
      } else if (value !== this.passwordForm.password) {
        callback(new Error('Inconsistent input password twice!'));
      } else {
        callback();
      }
    };

    return {
      passwordForm: {
        password: '',
        confirm: ''
      },
      passwordRules: {
        password: [{validator: validatePass}],
        confirm: [{validator: validatorConfirm}]
      },
      interfaces: []
    }
  },
  methods: {
    setPassword() {
      this.$session.get(r => {
        this.$system.setPassword(r.username, this.passwordForm.password).then(() => {
          this.$router.push('/login');
        });
      });
    }
  },
  created() {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces();
      this.interfaces = interfaces.map(item => item.interface);
    });
  }
}
</script>
