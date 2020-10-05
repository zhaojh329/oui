<script>
import OuiValidator from '@/plugins/oui-validator'
import OuiFormItemTemplate from './OuiFormItemTemplate.vue'

export default {
  inject: ['ouiForm', 'ouiSection'],
  components: {
    OuiFormItemTemplate
  },
  props: {
    label: String,
    required: Boolean,
    name: {
      type: String,
      required: true
    },
    uciSection: {
      type: Object,
      required: true
    },
    /* If load from uci fails, the value of the property is used as the form value. */
    initial: [String, Number, Array],
    /*
    ** If a function provided, the form loads the value by the function instead of from uci.
    ** Parameters: sid, self
    ** Return:  Promise object or value
    **
    ** If other type provided, the form loads the value from the prop's value.
    */
    load: [String, Number, Array, Function],
    /*
    ** If this function is provided, it will be called when oui saves the uci configuration.
    ** Parameters: self
    ** Return:  Promise object or undefined
    */
    save: Function,
    /*
    ** If this function is provided, it will be called when oui applys the uci configuration.
    ** Parameters: self
    ** Return:  Promise object or undefined
    */
    apply: Function,
    rules: {
      validator (value) {
        if (typeof value === 'string') return Object.keys(OuiValidator).indexOf(value) !== -1

        if (typeof value === 'object') {
          return Object.keys(OuiValidator).indexOf(value.type) !== -1
        }
        return typeof value === 'function'
      }
    },
    /* depend="(a == 12 || a == 'x') && y == 4 && q != 5 && !z" */
    depend: {
      type: String,
      default: ''
    },
    help: String
  },
  computed: {
    OuiFormItemTemplateProps () {
      return {
        visible: this.visible,
        label: this.label,
        prop: this.prop,
        help: this.help,
        rules: this.convertedRules
      }
    },
    config () {
      return this.ouiSection.config
    },
    form () {
      return this.ouiForm.form
    },
    model: {
      get: function () {
        return this.form[this.prop]
      },
      set: function (val) {
        this.form[this.prop] = val
      }
    },
    sid () {
      return this.uciSection['.name']
    },
    prop () {
      return `${this.sid}_${this.name}`
    },
    convertedRules () {
      const rules = []

      if (this.required) { rules.push({ required: true, message: () => this.$t('validator.required') }) }

      if (typeof this.rules === 'string') {
        const rule = {
          validator: (rule, value) => {
            if (!value) return true
            return OuiValidator[this.rules](value)
          },
          message: () => this.$t(`validator.${this.rules}`)
        }

        rules.push(rule)
      } else if (typeof this.rules === 'object') {
        rules.push({
          validator: (rule, value) => {
            if (!value) return true
            return OuiValidator[this.rules.type](value)
          },
          message: () => this.$t(`validator.${this.rules.type}`)
        })

        if (['number', 'integer', 'uinteger'].indexOf(this.rules.type) !== -1) {
          if (typeof this.rules.min === 'number') {
            rules.push({
              validator: (rule, value) => {
                if (!value) return true
                return value >= this.rules.min
              },
              message: () => this.$t('validator.min', { min: this.rules.min })
            })
          }

          if (typeof this.rules.max === 'number') {
            rules.push({
              validator: (rule, value) => {
                if (!value) return true
                return value <= this.rules.max
              },
              message: () => this.$t('validator.max', { max: this.rules.max })
            })
          }
        }
      } else if (typeof this.rules === 'function') {
        rules.push({
          validator: (rule, value, cb) => {
            if (!value) return true
            cb(this.rules(value, this))
          }
        })
      }

      return rules
    },
    __depend () {
      const compares = ['==', '===', '!=', '!==', '<', '>', '>=', '<=']
      const expr = this.depend.trim().replace(/\s+/g, ' ')
      const states = {
        name: 0,
        operand: 1,
        logic: 2,
        logicOrCmp: 3
      }

      if (expr === '') return undefined

      const parts = expr.split(' ')

      let waitRightBracket = 0
      let s = states.name
      const names = {}

      for (let i = 0; i < parts.length; i++) {
        let part = parts[i]

        if (s === states.name) {
          if (part[0] === '(') {
            waitRightBracket++
            part = part.substr(1)
          }

          if (part[part.length - 1] === ')') {
            waitRightBracket--
            part = part.substr(0, part.length - 1)
          }

          if (part[0] === '!') {
            part = part.substr(1)
            s = states.logic
          } else {
            s = states.logicOrCmp
          }

          if (!/^[A-Za-z_]/.test(part)) { return undefined }
          names[part] = true

          if (i === parts.length - 1) { return { expr, names: Object.keys(names) } }

          continue
        }

        if (s === states.logicOrCmp) {
          if (i === parts.length - 1) { return undefined }

          if (compares.indexOf(part) > -1) {
            s = states.operand
            continue
          }

          if (part === '||' || part === '&&') {
            s = states.name
            i++
            continue
          }

          return undefined
        }

        if (s === states.operand) {
          s = states.logic

          if (part[part.length - 1] === ')') {
            waitRightBracket--
            part = part.substr(0, part.length - 1)
          }

          const starts = part[0]
          const end = part[part.length - 1]

          if (starts === '\'' && end === '\'') {
            part = part.substr(1, part.length - 2)
            if (typeof (part) !== 'string') { return undefined }
            continue
          }

          if (starts !== '\'' && end !== '\'') {
            if (isNaN(Number(part))) { return undefined }
            continue
          }

          return undefined
        }

        if (s === states.logic) {
          if (i === parts.length - 1) { return undefined }

          if (part === '||' || part === '&&') {
            s = states.name
            continue
          }

          return undefined
        }

        return undefined
      }

      if (waitRightBracket !== 0) { return undefined }

      return { expr, names: Object.keys(names) }
    },
    dependExprValue () {
      return `'${this.model}'`
    },
    visible () {
      const depend = this.__depend
      if (!depend) return true

      let expr = depend.expr

      for (const i in depend.names) {
        const name = depend.names[i]
        const f = this.ouiForm.fields[`${this.sid}_${name}`]
        if (!f || !f.visible) return false
        expr = expr.replace(new RegExp(name, 'gm'), f.dependExprValue)
      }

      const Fn = Function
      const r = new Fn(`return ${expr}`)()

      return !!r
    }
  },
  methods: {
    convertUciValue (value) {
      return value
    },
    changed () {
      return this.initialValue !== this.model
    },
    reset () {
      this.model = this.initialValue
    },
    __save () {
      if (this.model === '' && this.initial) {
        this.model = this.initial
        return
      }

      if (this.changed()) {
        if (this.save) { return this.save(this) }
        this.$uci.set(this.config, this.sid, this.name, this.model)
      }
    },
    __apply () {
      if (!this.changed() || !this.apply) return
      return this.apply(this)
    },
    notifyChanged () {
      this.$emit('change', this)
      this.ouiSection.$emit(`change-${this.name}`, this)
      this.ouiSection.$emit('change', this.sid, this.ouiSection)
    },
    watchValue () {
      this.$watch(`form.${this.prop}`, _ => {
        this.notifyChanged()
      })
    },
    buildFormPost (value) {
      this.$set(this.form, this.prop, this.convertUciValue(value))
      Object.defineProperty(this, 'initialValue', {
        value: this.model
      })
      this.watchValue()
    },
    buildForm () {
      if (typeof (this.load) === 'function') {
        const r = this.load(this)
        if (window.oui.isPromise(r)) {
          r.then(value => this.buildFormPost(value))
        } else {
          this.buildFormPost(r)
        }
      } else if (typeof (this.load) !== 'undefined') {
        this.buildFormPost(this.load)
        this.$watch('load', value => {
          this.model = value
        })
      } else {
        let value = this.uciSection[this.name]
        if (typeof value === 'undefined') value = this.initial
        this.buildFormPost(value)
      }
    }
  },
  created () {
    if (this.ouiForm.fields[this.name]) {
      throw new Error(`Duplicate OuiFormItem name: ${this.name}`)
    }

    this.$set(this.ouiForm.fields, this.prop, this)

    this.buildForm()
  },
  beforeDestroy () {
    this.$delete(this.ouiForm.fields, this.prop)
  }
}
</script>
