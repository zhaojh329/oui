<template>
  <div :class="{ 'has-error': !!error }">
    <a-input ref="input" :placeholder="placeholder" @pressEnter="pressEnter" @change="changed" v-model="value"/>
    <div v-if="!!error" style="color: red">{{ error }}</div>
  </div>
</template>

<script>
export default {
  name: 'Prompt',
  props: {
    placeholder: String,
    validator: Function
  },
  data () {
    return {
      error: null,
      value: ''
    }
  },
  methods: {
    pressEnter () {
      this.$emit('enter', {})
    },
    changed () {
      this.$emit('change', this.value)

      if (!this.validator) return

      if (!this.value) {
        this.error = null
        this.$emit('validator', true)
      } else {
        this.error = this.validator(this.value)
        this.$emit('validator', !this.error)
      }
    }
  },
  mounted () {
    this.$nextTick(() => this.$refs.input.focus())
  }
}
</script>
