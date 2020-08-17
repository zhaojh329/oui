<template>
  <component :is="container" :title="title">
    <slot v-if="section"  :s="section"/>
  </component>
</template>

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
    card: {
      type: Boolean,
      default: true
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
    uciConfig () {
      return this.ouiForm.uciConfig
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
  }
}
</script>
