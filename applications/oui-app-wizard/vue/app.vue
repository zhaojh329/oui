<template>
  <div style="padding: 20px">
    <a-steps :current="current">
      <a-step v-for="(s, i) in steps" :key="i" :title="$t(s)"/>
    </a-steps>
    <div class="steps-content">
      <a-card :title="$t(steps[current])">
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
            <a-form-model-item :label="$t('wizard.Username')">
              <a-input value="admin" readonly/>
            </a-form-model-item>
            <a-form-model-item :label="$t('wizard.Password')" prop="password">
              <a-input-password v-model="form.password"/>
            </a-form-model-item>
            <a-form-model-item :label="$t('wizard.Confirmation')" prop="confirm">
              <a-input-password v-model="form.confirm"/>
            </a-form-model-item>
          </div>
        </a-form-model>
      </a-card>
      <div class="steps-action">
        <a-button type="primary" :disabled="current === 0" @click="prev">{{ $t('wizard.Back') }}</a-button>
        <a-button type="primary" v-if="current < steps.length - 1" @click="next">{{ $t('wizard.Next') }}</a-button>
        <a-button type="primary" v-if="current === steps.length - 1" @click="submit">{{ $t('wizard.Submit') }}</a-button>
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
      steps: [
        'wizard.Select Your Language',
        'wizard.Set administrator password'
      ],
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
        this.$rpc.call('ui', 'load_locales', { locale: lang }).then(locales => {
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
        this.$rpc.call('ui', 'first_set', {
          lang: this.form.lang,
          username: 'admin',
          password: this.form.password
        }).then(() => {
          sessionStorage.setItem('__oui_first_login', 'false')
          this.$router.push('/login')
        })
      })
    }
  }
}
</script>
<style scoped lang="stylus">
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
  > * {
    margin-right: 20px;
  }
}
</style>
