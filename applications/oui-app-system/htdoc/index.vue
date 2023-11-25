<template>
  <el-form ref="form" size="large" label-width="auto" label-suffix=":" :model="formValue" :rules="rules">
    <el-form-item :label="$t('Hostname')" prop="hostname">
      <el-input v-model="formValue.hostname"/>
    </el-form-item>
    <el-form-item :label="$t('Timezone')" prop="zonename">
      <el-select v-model="formValue.zonename" filterable>
        <el-option v-for="item in zoneinfo" :key="item[0]" :label="item[0]" :value="item[0]"/>
      </el-select>
    </el-form-item>
  </el-form>
  <div style="text-align: right; padding-right: 100px">
    <el-button type="primary" :loading="loading" @click="handleSubmit">{{ $t('Save & Apply') }}</el-button>
  </div>
</template>

<script>
import zoneinfo from './zoneinfo'

export default {
  data() {
    return {
      loading: false,
      zoneinfo: zoneinfo,
      formValue: {
        hostname: this.$oui.state.hostname,
        zonename: ''
      },
      rules: {
        hostname: {
          required: true,
          trigger: 'blur',
          validator: (_, value, callback) => {
            if (!value)
              return callback(new Error(this.$t('This field is required')))

            if (value.length <= 253 && (value.match(/^[a-zA-Z0-9_]+$/) || (value.match(/^[a-zA-Z0-9_][a-zA-Z0-9_\-.]*[a-zA-Z0-9]$/) && value.match(/[^0-9.]/))))
              return callback()
            return callback(new Error(this.$t('Invalid hostname')))
          }
        }
      }
    }
  },
  created() {
    this.$oui.call('uci', 'get', {
      config: 'system',
      section: '@system[0]',
      option: 'zonename'
    }).then(zonename => {
      this.formValue.zonename = zonename || 'UTC'
    })
  },
  methods: {
    handleSubmit() {
      this.$refs.form.validate(async valid => {
        if (!valid)
          return

        this.loading = true

        await this.$oui.setHostname(this.formValue.hostname)

        let timezone = zoneinfo.filter(item => item[0] === this.formValue.zonename)[0][1]
        let zonename = this.formValue.zonename

        if (zonename === 'UTC')
          zonename = ''

        await this.$oui.call('uci', 'set', {
          config: 'system',
          section: '@system[0]',
          values: { timezone, zonename }
        })

        await this.$oui.reloadConfig('system')

        this.loading = false

        this.$message.success(this.$t('Configuration has been applied'))
      })
    }
  }
}
</script>

<i18n src="./locale.json"/>
