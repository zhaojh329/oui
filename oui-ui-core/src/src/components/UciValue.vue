<template>
  <el-form-item :label="label" :prop="formItemProp" :rules="rules">
    <el-input v-if="type === 'input'" :readonly="readonly" v-model="ivalue" :placeholder="placeholder"></el-input>
    <el-switch v-else-if="type === 'switch'" v-model="ivalue" active-value="1" inactive-value="0"></el-switch>
    <el-select v-else-if="type === 'list'" v-model="ivalue">
      <el-option v-for="item in options" :key="item[0]" :label="item[1] || item[0]" :value="item[0]"></el-option>
    </el-select>
  </el-form-item>
</template>

<script>

export default {
  name: 'UciValue',
  props: {
    /* component type */
    type: {
      type: String,
      validator: function(value) {
        return ['input', 'switch', 'list'].indexOf(value) !== -1;
      }
    },
    /* uci option name or a normal form item name */
    name: String,
    /* form item label */
    label: String,
    required: Boolean,
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: String,
    placeholder: String,
    readonly: Boolean,
    /* Used for list */
    options: Array,
    /* Whether the name property is a uci property */
    uci: {
      type: Boolean,
      default: true
    },
    /* If this function is provided, the form loads the value by the function instead of from uci */
    onLoad: Function,
    /* If this function is provided, it will be called when the user clicks the apply button */
    onSave: Function
  },
  data() {
    return {
      ivalue: '',
      iinitial: null
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
    },
    formItemProp() {
      return `${this.sid}-${this.name}`;
    },
    rules() {
      const rule = [];

      if (this.required)
        rule.push({required: true, message: `${this.name} is required`});

      return rule;
    }
  },
  watch: {
    sid() {
      if (typeof(this.name) === 'undefined')
        return;

      let tab = undefined;
      const tabPane = this.$getParent('ElTabPane', 3);
      if (tabPane !== null)
        tab = tabPane.name;

      this.$getParent('UciForm').addFormItem({
        name: this.formItemProp,
        tab: tab,
        onSave: this.onSave
      });
    },
    ivalue(v) {
      if (typeof(this.name) === 'undefined')
        return;

      this.$getParent('UciForm').form[this.formItemProp] = v;

      if (v !== this.iinitial && this.uci)
        this.$uci.set(this.config, this.sid, this.name, v);
    },
    loaded() {
      if (typeof(this.onLoad) !== 'undefined') {
        new Promise(resolve => {
          this.onLoad(resolve);
        }).then(v => {
          this.ivalue = v;
          this.updateInitial();
        });
        return;
      }

      if (typeof(this.name) === 'undefined')
        return;

      if (this.uci)
        this.ivalue = this.$uci.get(this.config, this.sid, this.name) || this.initial;

      this.updateInitial();
    }
  },
  methods: {
    updateInitial() {
      this.iinitial = this.ivalue;
      this.$getParent('UciForm').initials[this.formItemProp] = this.ivalue;
    }
  }
}
</script>