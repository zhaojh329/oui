<template>
  <el-tabs value="password">
    <el-tab-pane :label="$t('Router Password')" name="password">
      <el-card :title="$t('Router Password')" style="width: 500px">
        <el-form label-width="200px" :model="passwordForm" :rules="passwordRules" ref="passwordForm">
          <el-form-item :label="$t('Password')" prop="password">
            <el-input show-password v-model="passwordForm.password"></el-input>
          </el-form-item>
          <el-form-item :label="$t('Confirmation')" prop="confirm">
            <el-input show-password v-model="passwordForm.confirm"></el-input>
          </el-form-item>
          <el-form-item>
            <el-button type="primary" size="mini"  @click="setPassword" style="float: right;margin-right: 50px;">{{ $t('Save') }}</el-button>
          </el-form-item>
        </el-form>
      </el-card>
    </el-tab-pane>
    <el-tab-pane :label="$t('SSH Access')" name="dropbear" v-if="hasDropbear">
      <uci-form config="dropbear">
        <uci-section :title="$t('SSH Server')" type="dropbear" addable>
          <uci-option-list :label="$t('Interface')" name="Interface" :options="interfaces" :description="$t('Listen only on the given interface or, if unspecified, on all')"></uci-option-list>
          <uci-option-input :label="$t('Port')" name="Port" placeholder="22" rules="port" :description="$t('Specifies the listening port of this Dropbear instance')"></uci-option-input>
          <uci-option-switch :label="$t('Password authentication')" name="PasswordAuth" initial="on" active-value="on" inactive-value="off" :description="$t('Allow SSH password authentication')"></uci-option-switch>
          <uci-option-switch :label="$t('Allow root logins with password')" name="RootPasswordAuth" initial="on" active-value="on" inactive-value="off" :description="$t('Allow the root user to login with password')"></uci-option-switch>
          <uci-option-switch :label="$t('Gateway ports')" name="GatewayPorts" active-value="on" inactive-value="off" :description="$t('Allow remote hosts to connect to local SSH forwarded ports')"></uci-option-switch>
        </uci-section>
      </uci-form>
    </el-tab-pane>
  </el-tabs>
</template>

<script>

export default {
  data() {
    const validatePass = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('Please enter your password')));
      } else {
        if (this.passwordForm.confirm !== '')
          this.$refs['passwordForm'].validateField('confirm');
        callback();
      }
    };

    const validatorConfirm = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('Please enter your password again')));
      } else if (value !== this.passwordForm.password) {
        callback(new Error(this.$t('Inconsistent input password twice!')));
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
      interfaces: [
        ['', this.$t('unspecified')]
      ],
      hasDropbear: false
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
      this.interfaces.push(...interfaces.map(item => item.name));
    });

    this.$uci.load('dropbear').then(() => {
      this.hasDropbear = true;
    }).catch(() => {
      this.hasDropbear = false;
    });
  }
}
</script>
