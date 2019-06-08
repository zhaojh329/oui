<template>
  <el-form-item :label="option.label" :prop="prop" v-if="visible">
    <el-input v-if="type === 'input'" v-model="form[prop]" :placeholder="option.placeholder"></el-input>
    <el-input v-else-if="type === 'dummy'" :value="form[prop]" readonly></el-input>
    <el-switch v-else-if="type === 'switch'" v-model="form[prop]" :active-value="option.activeValue" :inactive-value="option.inactiveValue"></el-switch>
    <el-select v-else-if="type === 'list'" v-model="form[prop]" :clearable="!option.required">
      <el-option v-for="oo in option.transformedOptions" :key="oo[0]" :label="oo[1] || oo[0]" :value="oo[0]"></el-option>
    </el-select>
    <uci-dlist v-else-if="type === 'dlist'" v-model="form[prop]"></uci-dlist>
  </el-form-item>
</template>

<script>
import UciDlist from './uci-dlist'

export default {
  name: 'UciFormItem',
  props: {
    sid: String,
    option: Object,
    form: Object
  },
  components: {
    UciDlist
  },
  computed: {
    prop() {
      return this.option.prop(this.sid);
    },
    type() {
      return this.option.type;
    }
  },
  data() {
    return {
      visible: true
    }
  }
}
</script>
