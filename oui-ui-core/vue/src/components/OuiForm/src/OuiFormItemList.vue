<template>
  <oui-form-item-template v-bind="OuiFormItemTemplateProps" ref="form-item" :rules="listRules">
    <div class="oui-form-list">
      <a-tag v-for="tag in model" :key="tag" closable @close="handleDelDlist(tag)">{{ tag }}</a-tag>
      <template v-if="inputVisible">
        <a-input size="small" ref="input" v-model="inputValue" @change="validate" @blur="handleinputConfirm" @pressEnter="handleinputConfirm"/>
      </template>
      <a-button v-else size="small" type="primary" @click="showInput" style="width: 100px">+ {{ $t('Add') }}</a-button>
    </div>
  </oui-form-item-template>
</template>

<script>
import OuiFormItemMixin from './OuiFormItemMixin'

export default {
  name: 'OuiFormItemList',
  mixins: [OuiFormItemMixin],
  props: {
    initial: {
      validator (value) {
        if (!Array.isArray(value)) { return false }
        for (const item in value) {
          if (typeof item !== 'number' && typeof item !== 'string') { return false }
        }
        return true
      }
    }
  },
  data () {
    return {
      inputVisible: false,
      inputValue: '',
      error: null
    }
  },
  computed: {
    listRules () {
      return {
        type: 'array',
        fields: this.model.map(() => this.convertedRules)
      }
    }
  },
  methods: {
    changed () {
      return this.initialValue.sort().toString() !== this.model.sort().toString()
    },
    convertUciValue (value) {
      if (typeof value === 'undefined') { return [] } else { return Array.from(value) }
    },
    validate () {
      if (this.model.indexOf(this.inputValue) > -1) return

      this.model.push(this.inputValue)

      this.$nextTick(() => this.ouiForm.validateField(this.prop, errMsg => {
        this.error = errMsg
        this.model.splice(-1)
      }))
    },
    handleDelDlist (tag) {
      const index = this.model.indexOf(tag)
      this.model.splice(index, 1)
    },
    showInput () {
      this.inputVisible = true
      this.$nextTick(() => {
        this.$refs.input.focus()
      })
    },
    handleinputConfirm () {
      if (this.error) {
        this.$nextTick(() => this.ouiForm.validateField(this.prop))
        this.error = null
      } else if (this.inputValue && this.model.indexOf(this.inputValue) === -1) {
        this.model.push(this.inputValue)
      }

      this.inputValue = ''
      this.inputVisible = false
    }
  }
}
</script>

<style lang="stylus">
.oui-form-list {
  .ant-tag {
    display: block;

    i {
      margin-left: 20px;
    }
  }

  > * {
    margin-bottom: 5px;
  }
}
</style>
