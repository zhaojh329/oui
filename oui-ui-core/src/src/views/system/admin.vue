<template>
  <a-tabs>
    <a-tab-pane key="password" :tab="$t('Router Password')">
      <a-form-model :model="passwordForm" :rules="passwordRules" ref="passwordForm" :label-col="labelCol" :wrapper-col="wrapperCol">
        <a-form-model-item :label="$t('Password')" prop="password">
          <a-input-password v-model="passwordForm.password"/>
        </a-form-model-item>
        <a-form-model-item :label="$t('Confirmation')" prop="confirm">
          <a-input-password v-model="passwordForm.confirm"/>
        </a-form-model-item>
        <a-form-model-item>
          <a-button type="primary" @click="setPassword" style="float: right;margin-right: 50px;">{{ $t('Save') }}</a-button>
        </a-form-model-item>
      </a-form-model>
    </a-tab-pane>
    <a-tab-pane key="ssh" :tab="$t('SSH Access')" v-if="hasDropbear">
      <oui-form uci-config="dropbear">
        <oui-typed-section :title="$t('SSH Server')" type="dropbear" v-slot="{ s }" addremove>
          <oui-form-item-select :uci-section="s" :label="$t('Interface')" name="Interface" :options="interfaces" :help="$t('Listen only on the given interface or, if unspecified, on all')"/>
          <oui-form-item-input :uci-section="s" :label="$t('Port')" name="Port" placeholder="22" rules="port" :help="$t('Specifies the listening port of this Dropbear instance')"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Password authentication')" name="PasswordAuth" initial="on" true-value="on" false-value="off" :help="$t('Allow SSH password authentication')"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Allow root logins with password')" name="RootPasswordAuth" initial="on" true-value="on" false-value="off" :help="$t('Allow the root user to login with password')"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Gateway ports')" name="GatewayPorts" true-value="on" false-value="off" :help="$t('Allow remote hosts to connect to local SSH forwarded ports')"/>
        </oui-typed-section>
      </oui-form>
    </a-tab-pane>
  </a-tabs>
</template>

<script>

export default {
  data () {
    const validatePass = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('Please enter your password')))
      } else {
        if (this.passwordForm.confirm !== '') { this.$refs.passwordForm.validateField('confirm') }
        callback()
      }
    }

    const validatorConfirm = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('Please enter your password again')))
      } else if (value !== this.passwordForm.password) {
        callback(new Error(this.$t('Inconsistent input password twice!')))
      } else {
        callback()
      }
    }

    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },
      passwordForm: {
        password: '',
        confirm: ''
      },
      passwordRules: {
        password: [{ validator: validatePass }],
        confirm: [{ validator: validatorConfirm }]
      },
      interfaces: [
        ['', this.$t('unspecified')]
      ],
      hasDropbear: false
    }
  },
  methods: {
    setPassword () {
      this.$session.get(r => {
        this.$system.setPassword(r.username, this.passwordForm.password).then(() => {
          this.$router.push('/login')
        })
      })
    }
  },
  created () {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces()
      this.interfaces.push(...interfaces.map(item => item.name))
    })

    this.$uci.load('dropbear').then(() => {
      this.hasDropbear = true
    }).catch(() => {
      this.hasDropbear = false
    })
  }
}
</script>
