<template>
  <a-form-model :model="form" :rules="rules" ref="form" :label-col="labelCol" :wrapper-col="wrapperCol">
    <a-form-model-item :label="$t('admin.Password')" prop="password">
      <a-input-password v-model="form.password"/>
    </a-form-model-item>
    <a-form-model-item :label="$t('admin.Confirmation')" prop="confirm">
      <a-input-password v-model="form.confirm"/>
    </a-form-model-item>
    <a-form-model-item>
      <a-button type="primary" @click="setPassword" style="float: right;margin-right: 50px;">{{ $t('Save') }}</a-button>
    </a-form-model-item>
  </a-form-model>
</template>

<script>

export default {
  data () {
    const validatePass = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('Please enter your password')))
      } else {
        if (this.form.confirm !== '') { this.$refs.form.validateField('confirm') }
        callback()
      }
    }

    const validatorConfirm = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('admin.Please enter your password again')))
      } else if (value !== this.form.password) {
        callback(new Error(this.$t('admin.Inconsistent input password twice!')))
      } else {
        callback()
      }
    }

    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 10 },
      form: {
        password: '',
        confirm: ''
      },
      rules: {
        password: [{ validator: validatePass }],
        confirm: [{ validator: validatorConfirm }]
      }
    }
  },
  methods: {
    setPassword () {
      this.$refs.form.validate(valid => {
        if (!valid) return
        this.$rpc.call('oui', 'set_password', { password: this.$md5(this.form.password) }).then(() => {
          this.$router.push('/login')
        })
      })
    }
  }
}
</script>
