<template>
  <el-tag v-for="tag in modelValue" :key="tag" style="margin-right: 10px;" closable @close="handleClose(tag)">{{ tag }}</el-tag>
  <el-input v-if="inputVisible" ref="InputRef" v-model="inputValue" style="width: 200px;" size="small"
    @keyup.enter="handleInputConfirm" @blur="handleInputConfirm"/>
  <el-button v-else class="button" size="small" @click="showInput">+</el-button>
</template>

<script>
import { nextTick } from 'vue'

export default {
  props: {
    modelValue: {
      type: Array,
      required: true
    }
  },
  emits: ['update:modelValue'],
  data() {
    return {
      inputVisible: false,
      inputValue: ''
    }
  },
  methods: {
    handleClose(tag) {
      this.$emit('update:modelValue', this.modelValue.filter(x => x !== tag))
    },
    showInput() {
      this.inputVisible = true
      nextTick(() => this.$refs.InputRef.input.focus())
    },
    handleInputConfirm() {
      if (this.inputValue)
        this.$emit('update:modelValue', [...this.modelValue, this.inputValue])
      this.inputVisible = false
      this.inputValue = ''
    }
  }
}
</script>
