<template>
  <div v-if="visible">
    <el-form-item v-if="nameVisible" size="mini" label-width="auto" style="width: 200px" :prop="prop" :rules="rules">
      <el-input placeholder="Please input a name" v-model="form[prop]" ref="name" @keyup.enter.native="handleNameConfirm" @blur="handleNameConfirm"></el-input>
    </el-form-item>
    <el-button v-else type="primary" size="mini" @click="add">+ {{ $t('Add') }}</el-button>
  </div>
</template>

<script>
export default {
  name: 'UciSectionAdd',
  inject: ['uciForm'],
  props: {
    sestion: Object,
    form: Object
  },
  computed: {
    visible() {
      const s = this.sestion;
      return s.addable && s.type && !s.name;
    },
    prop() {
      return 'uci_sestion_' + this.sestion.type;
    }
  },
  data() {
    return {
      nameVisible: false,
      rules: [
        {validator: this.validateName}
      ],
      valid: false
    }
  },
  created() {
    this.$set(this.uciForm.form, this.prop, '');
  },
  methods: {
    add() {
      if (this.sestion.anonymous) {
        this.sestion.add();
        return;
      }

      this.nameVisible = true;
      this.$nextTick(() => {
        this.$refs['name'].focus();
      });
    },
    handleNameConfirm() {
      const name = this.uciForm.form[this.prop];

      this.nameVisible = false;

      if (name === '')
        return;

      this.uciForm.form[this.prop] = '';

      if (this.valid)
        this.sestion.add(name);
    },
    validateName(rule, value, callback) {
      let error = undefined;

      this.sestion.uciSections.forEach(s => {
        if (s['.name'] === value)
          error = new Error('Name already used');
      });

      this.valid = !error;

      callback(error);
    }
  }
}
</script>
