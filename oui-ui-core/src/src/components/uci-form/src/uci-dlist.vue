<template>
  <div class="oui-uci-dlist">
    <el-tag :closable="!readonly" v-for="tag in value" :key="tag" :disable-transitions="false" @close="handleDelDlist(tag)">{{ tag }}</el-tag>
    <el-input v-if="inputVisible" size="small" ref="input" v-model="inputValue" @keyup.enter.native="handleinputConfirm" @blur="handleinputConfirm"></el-input>
    <el-button v-else size="mini" type="primary" plain @click="showInput">+ {{ $t('Add') }}</el-button>
  </div>
</template>

<script>
export default {
  name: 'UciDlist',
  inject: ['uciForm'],
  props: {
    value: {
      type: Array,
      default: () => {
        return [];
      }
    },
    prop: {
      type: String,
      required: true
    }
  },
  data() {
    return {
      inputVisible: false,
      inputValue: ''
    }
  },
  computed: {
    readonly() {
      return this.uciForm.readonly;
    },
    form() {
      return this.uciForm.form;
    }
  },
  methods: {
    handleDelDlist(tag) {
      let tags = [...this.value];
      tags.splice(tags.indexOf(tag), 1);
      this.$emit('input', tags);
    },
    showInput() {
      this.inputVisible = true;
      this.$nextTick(() => {
        this.$refs['input'].focus();
      });
    },
    clear() {
      this.inputVisible = false;
      this.inputValue = '';
    },
    handleinputConfirm() {
      if (this.inputValue) {
        const tags = [...this.value];

        if (tags.indexOf(this.inputValue) > -1) {
          this.clear();
          return;
        }

        tags.push(this.inputValue);
        this.$emit('input', tags);

        this.$nextTick(() => {
          this.uciForm.$refs['form'].validateField(this.prop, (err) => {
            if (err) {
              this.form[this.prop].splice(this.form[this.prop].indexOf(this.inputValue), 1);
            } else {
              this.clear();
            }
          })
        });
      } else {
        this.inputVisible = false;
      }
    }
  }
}
</script>

<style lang="scss">
.oui-uci-dlist {
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
