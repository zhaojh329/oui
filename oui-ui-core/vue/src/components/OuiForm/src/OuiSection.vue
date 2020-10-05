<script>
export default {
  inject: ['ouiForm'],
  provide () {
    return {
      ouiSection: this
    }
  },
  props: {
    title: String,
    uciConfig: String,
    card: {
      type: Boolean,
      default: true
    }
  },
  data () {
    return {
      loaded: false
    }
  },
  components: {
    OuiSectionContainerDiv: {
      render (h) {
        return h('div', this.$slots.default)
      }
    }
  },
  computed: {
    container () {
      if (this.card && !this.columns) return 'a-card'
      return 'oui-section-container-div'
    },
    config () {
      return this.uciConfig || this.ouiForm.uciConfig
    },
    form () {
      return this.ouiForm.form
    }
  },
  methods: {
    get (sid, name) {
      return this.form[`${sid}_${name}`]
    },
    set (sid, name, value) {
      this.form[`${sid}_${name}`] = value
    }
  },
  created () {
    if (this.uciConfig) {
      this.$uci.load(this.uciConfig).then(() => {
        this.loaded = true
      })
    } else {
      this.loaded = true
    }
  }
}
</script>
