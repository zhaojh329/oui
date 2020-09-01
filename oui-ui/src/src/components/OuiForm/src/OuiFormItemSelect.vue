<template>
  <oui-form-item-template v-bind="OuiFormItemTemplateProps">
    <a-select v-if="multiple || !allowCreate" v-model="model" :placeholder="placeholder" :mode="mode">
      <a-select-option v-for="o in convertedOptions" :key="o[0]" :value="o[0]">{{ o[1] }}</a-select-option>
    </a-select>
    <a-auto-complete v-else v-model="model" :placeholder="placeholder" :data-source="convertedDataSource" option-label-prop="value" allow-clear/>
  </oui-form-item-template>
</template>

<script>
import OuiFormItemMixin from './OuiFormItemMixin'

export default {
  name: 'OuiFormItemSelect',
  mixins: [OuiFormItemMixin],
  props: {
    options: Array,
    placeholder: String,
    multiple: Boolean,
    allowCreate: Boolean
  },
  computed: {
    mode () {
      if (this.multiple) {
        if (this.allowCreate) return 'tags'
        return 'multiple'
      }
      return null
    },
    convertedOptions () {
      const options = []
      this.options.forEach(o => {
        if (!Array.isArray(o)) { o = [o, o] } else if (o.length < 2) { o.push(o) }
        options.push(o)
      })
      return options
    },
    convertedDataSource () {
      const options = []
      this.options.forEach(o => {
        if (Array.isArray(o)) {
          options.push({
            text: o[1],
            value: o[0]
          })
        } else {
          options.push(o)
        }
      })
      return options
    }
  }
}
</script>
