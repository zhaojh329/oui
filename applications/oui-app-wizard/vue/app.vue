<template>
  <div style="padding: 20px">
    <a-steps :current="current">
      <a-step :title="$t('wizard.Select Language')"/>
      <a-step :title="$t('wizard.Set admin password')"/>
    </a-steps>
    <div class="steps-content">
      <a-form-model style="width: 400px" ref="form" :model="form" :rules="rules">
        <a-form-model-item prop="lang" v-if="current === 0">
          <a-select v-model="form.lang" @change="langChanged">
            <a-select-option value="en">English</a-select-option>
            <a-select-option value="zh-cn">简体中文</a-select-option>
            <a-select-option value="zh-tw">繁体中文</a-select-option>
            <a-select-option value="ja">日本語</a-select-option>
            <a-select-option value="auto">{{ $t('Automatic') }}</a-select-option>
          </a-select>
        </a-form-model-item>
        <div v-if="current === 1">
          <a-form-model-item :label="$t('wizard.Password')" prop="password">
            <a-input-password v-model="form.password"/>
          </a-form-model-item>
          <a-form-model-item :label="$t('wizard.Confirmation')" prop="confirm">
            <a-input-password v-model="form.confirm"/>
          </a-form-model-item>
          <a-form-model-item>
            <a-button style="float: right" type="primary" @click="submit">{{ $t('wizard.OK') }}</a-button>
          </a-form-model-item>
        </div>
      </a-form-model>
      <div class="steps-action">
        <a-button-group>
          <a-button type="primary" icon="left" :disabled="current === 0" @click="prev">{{ $t('wizard.Prev') }}</a-button>
          <a-button type="primary" :disabled="current === 1" @click="next">
            {{ $t('wizard.Next') }}<a-icon type="right"/>
          </a-button>
        </a-button-group>
      </div>
    </div>
  </div>
</template>
<script>
export default {
  data () {
    const validatePass = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('wizard.Please enter your password')))
      } else {
        if (this.form.confirm !== '') { this.$refs.form.validateField('confirm') }
        callback()
      }
    }

    const validatorConfirm = (rule, value, callback) => {
      if (value === '') {
        callback(new Error(this.$t('wizard.Please enter your password again')))
      } else if (value !== this.form.password) {
        callback(new Error(this.$t('wizard.Inconsistent input password twice!')))
      } else {
        callback()
      }
    }

    return {
      current: 0,
      form: {
        lang: 'auto',
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
    next () {
      this.current++
    },
    prev () {
      this.current--
    },
    langChanged (lang) {
      this.$store.commit('setLang', lang)

      if (lang === 'auto') { lang = navigator.language.toLowerCase() }

      if (lang === 'zh') lang = 'zh-cn'

      if (!this.$i18n.loaded(lang)) {
        this.$rpc.call('oui', 'load_locales', { locale: lang }).then(locales => {
          locales.forEach(locale => this.$i18n.mergeLocaleMessage(lang, locale))
          this.$i18n.locale = lang
        })
      } else {
        this.$i18n.locale = lang
      }
    },
    submit () {
      this.$refs.form.validate(valid => {
        if (!valid) return
        this.$rpc.call('oui', 'first_set', {
          lang: this.form.lang,
          password: this.$md5(this.form.password)
        }).then(() => {
          sessionStorage.setItem('__oui_first_login', 'false')
          this.$router.push('/login')
        })
      })
    }
  }
}
</script>
<style scoped>
.steps-content {
  padding-top: 100px;
  width: 500px;
  left: 50%;
  position: fixed;
  transform: translate(-50%, 0);
}

.steps-action {
  margin-top: 10px;
  text-align: center;
}
</style>
