<template>
  <a-form-model ref="form" :model="form" :label-col="labelCol" :wrapper-col="wrapperCol">
    <slot v-if="loaded"/>
    <slot name="footer">
      <a-form-model-item :colon="false">
        <template #label>
          <span/>
        </template>
        <div style="float: right">
          <a-button type="primary" style="margin-right: 10px" @click="apply">{{ $t('Save & Apply') }}</a-button>
          <a-button type="danger" @click="reset">{{ $t('Reset') }}</a-button>
        </div>
      </a-form-model-item>
    </slot>
  </a-form-model>
</template>

<script>
export default {
  name: 'OuiForm',
  provide () {
    return {
      ouiForm: this
    }
  },
  props: {
    uciConfig: String
  },
  data () {
    return {
      labelCol: { span: 6 },
      wrapperCol: { span: 14 },
      form: {},
      fields: {},
      loaded: false
    }
  },
  methods: {
    validate () {
      return new Promise(resolve => {
        this.$refs.form.validate(valid => resolve(valid))
      })
    },
    validateField (props, callback) {
      this.$refs.form.validateField(props, callback)
    },
    reset () {
      return new Promise(resolve => {
        this.loaded = false
        this.$uci.reset()
        this.form = {}

        this.$nextTick(() => {
          this.loaded = true
          resolve()
        })
      })
    },
    load () {
      return new Promise(resolve => {
        if (typeof this.uciConfig === 'string') {
          this.$uci.load(this.uciConfig).then(() => {
            this.loaded = true
            resolve()
          })
        } else {
          this.loaded = true
          resolve()
        }
      })
    },
    save () {
      return new Promise(resolve => {
        const promises = []

        Object.values(this.fields).forEach(item => promises.push(item.__save()))

        if (promises.length > 0) {
          Promise.all(promises).then(() => {
            resolve()
          })
        } else {
          resolve()
        }
      })
    },
    apply () {
      this.validate().then(valid => {
        if (!valid) return

        this.save().then(() => {
          const promises = []

          if (this.$uci.changed() > 0) {
            const p = new Promise(resolve => {
              this.$uci.save().then(() => {
                this.$uci.apply().then(() => resolve())
              })
            })

            promises.push(p)
          }

          Object.values(this.fields).forEach(item => {
            const p = item.__apply()
            if (window.oui.isPromise(p)) promises.push(p)
          })

          if (promises.length === 0) {
            this.$message.warning(this.$t('There are no changes to apply'))
            return
          }

          this.$spin(this.$t('Waiting for configuration to be applied...'))

          Promise.all(promises).then(() => {
            this.$uci.reset()
            this.load().then(() => {
              this.$spin(false)
              this.$emit('applied')
              this.reset().then(() => this.$message.success(this.$t('Configuration has been applied')))
            })
          })
        })
      })
    }
  },
  created () {
    this.$spin()
    this.load().then(() => this.$spin(false))
  },
  destroyed () {
    this.$uci.reset()
  }
}
</script>
