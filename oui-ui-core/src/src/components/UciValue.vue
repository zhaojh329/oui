<template>
  <el-form-item :label="label" :prop="formItemProp" :rules="rules" v-if="show">
    <el-input v-if="type === 'input'" :readonly="readonly" v-model="ivalue" :placeholder="placeholder"></el-input>
    <el-switch v-else-if="type === 'switch'" v-model="ivalue" :active-value="activeValue" :inactive-value="inactiveValue"></el-switch>
    <el-select v-else-if="type === 'list'" v-model="ivalue">
      <el-option v-for="item in options" :key="item[0]" :label="item[1] || item[0]" :value="item[0]"></el-option>
    </el-select>
    <div v-else-if="type === 'dlist'" class="dlist">
      <el-tag closable v-for="tag in ivalue" :key="tag" :disable-transitions="false" @close="handleDelDlist(tag)">{{ tag }}</el-tag>
      <el-input v-if="inputDlistVisible" size="small" ref="inputDlist" v-model="inputDlistValue" @keyup.enter.native="handleInputDlistConfirm" @blur="handleInputDlistConfirm"></el-input>
      <el-button v-else size="small" @click="showInputDlist">+ Add</el-button>
    </div>
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
        return ['input', 'switch', 'list', 'dlist'].indexOf(value) !== -1;
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
    onSave: Function,
    depends: [String, Object],
    activeValue: {
      type: String,
      default: '1'
    },
    inactiveValue: {
      type: String,
      default: '0'
    }
  },
  data() {
    return {
      show: true,
      ivalue: null,
      iinitial: null,
      inputDlistVisible: false,
      inputDlistValue: ''
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
    ivalue() {
      this.updateFormValue();
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
      if (Array.isArray(this.ivalue)) {
        this.iinitial = Object.assign([], this.ivalue);
        this.$getParent('UciForm').initials[this.formItemProp] = Object.assign([], this.ivalue);
      } else {
        this.iinitial = this.ivalue;
        this.$getParent('UciForm').initials[this.formItemProp] = this.ivalue;
      }
    },
    toggleDepends() {
      const uciValues = this.$getParent('UciSection').uciValues;

      Object.keys(uciValues).forEach(key => {
        const uciValue = uciValues[key];
        let depends = uciValue.depends;

        if (uciValue === this)
          return;

        if (typeof(depends) === 'undefined')
          return;

        if (typeof(depends) === 'string')
          depends = {[depends]: '1'};

        if (Object.keys(depends).indexOf(this.name) > -1) {
          let show = true;

          for (let name in depends) {
            if (depends[name] !== uciValues[name].ivalue) {
              show = false;
              break;
            }
          }

          uciValue.show = show;
        }
      });
    },
    updateFormValue() {
      if (typeof(this.name) === 'undefined')
        return;

      this.$getParent('UciForm').form[this.formItemProp] = this.ivalue;

      if (this.ivalue !== this.iinitial && this.uci)
        this.$uci.set(this.config, this.sid, this.name, this.ivalue);

      this.toggleDepends();
    },
    handleDelDlist(tag) {
      this.ivalue.splice(this.ivalue.indexOf(tag), 1);
    },
    showInputDlist() {
      this.inputDlistVisible = true;
      this.$nextTick(() => {
        this.$refs['inputDlist'].focus();
      });
    },
    handleInputDlistConfirm() {
      let inputValue = this.inputDlistValue;
      if (inputValue)
        this.ivalue.push(inputValue);
      this.inputDlistVisible = false;
      this.inputDlistValue = '';
    }
  },
  created() {
    const s = this.$getParent('UciSection');
    s.uciValues[this.name] = this;
  }
}
</script>

<style lang="scss" scoped>
.dlist {
  display: flex;
  flex-direction: column;

  > * {
    margin-bottom: 5px;
  }

  .el-button {
    width: 100px;
  }
}
</style>
