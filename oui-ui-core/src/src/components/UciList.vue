<template>
  <el-form-item :label="label" :prop="name">
    <el-select v-model="ivalue">
      <el-option v-for="item in options" :key="item[0]" :label="item[1] || item[0]" :value="item[0]"></el-option>
    </el-select>
  </el-form-item>
</template>

<script>

export default {
  name: 'UciList',
  props: {
    tab: String,
    label: String,
    name: String,
    value: String,
    initial: String,
    options: Array
  },
  data() {
    return {
      ivalue: '',
      iinitial: ''
    }
  },
  computed: {
    config() {
      return this.$getParent('UciForm').config;
    },
    sid() {
      return this.$getParent('UciSection').sid;
    },
    loaded() {
      return this.$getParent('UciForm').loaded;
    }
  },
  created() {
    if (typeof(this.name) === 'undefined')
      return;
    this.$getParent('UciForm').addFormItem(this.name, this.tab);
  },
  watch: {
    ivalue(v) {
      if (typeof(this.name) === 'undefined')
        return;

      this.$getParent('UciForm').form[this.name] = v;

      if (v !== this.iinitial)
        this.$uci.set(this.config, this.sid, this.name, v);
    },
    value(v) {
      this.ivalue = v;
    },
    loaded(n) {
      if (typeof(this.value) !== 'undefined') {
        this.ivalue = this.value;
        return;
      }

      if (typeof(this.name) === 'undefined')
        return;

      this.ivalue = this.$uci.get(this.config, this.sid, this.name) || this.initial;

      if (n === 1)
        this.iinitial = this.ivalue;
    }
  }
}
</script>