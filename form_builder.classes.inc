<?php

/**
 * This class is a wrapper around all the hooks used for getting pluigns.
 *
 * Currently supported plugin-types are:
 * - form types: hook_form_builder_form_types().
 * - element types: hook_form_builder_types().
 * - properties: hook_form_builder_properties().
 */
class FormBuilderLoader {

  protected static $instance = NULL;
  protected $formTypeInfo;
  protected $elementTypeInfo = array();
  protected $paletteGroupInfo = array();
  protected $propertyInfo = array();
  protected $formTypes = array();

  /**
   * Get a singleton-like class instance.
   */
  public static function instance() {
    if (!static::$instance) {
      static::$instance = new static();
    }
    return static::$instance;
  }

  public function __construct() {
    $this->loadFormTypeInfo();
  }

  protected function loadFormTypeInfo() {
    $defaults = array(
      'class' => 'FormBuilderFormBase',
      'property class' => 'FormBuilderPropertyBase',
      'element class' => 'FormBuilderElementBase',
    );

    $form_types = module_invoke_all('form_builder_form_types');
    $element_types = $this->invokeElementTypeInfo();
    foreach (array_keys($element_types) as $form_type) {
      $form_types += array($form_type => array());
    }

    foreach ($form_types as $form_type => &$info) {
      $info += $defaults;
      $element_types += array($form_type => array());
      foreach ($element_types[$form_type] as &$element_type) {
        $element_type += array('class' => $info['element class']);
      }
    }

    $this->formTypeInfo = $form_types;
    $this->elementTypeInfo = $element_types;
  }

  protected function invokeElementTypeInfo() {
    $form_types = module_invoke_all('form_builder_types');
    foreach ($form_types as $form_type => &$element_types) {
      $groups = $this->getPaletteGroupInfo($form_type);
      // Add default values for undefined properties.
      foreach ($element_types as $key => &$type) {
        $type += array(
          'configurable' => TRUE,
          'removable' => TRUE,
          'palette_group' => 'default',
          'properties' => array(),
        );
        $type += array(
          'addable' => $type['removable'] && isset($type['default']),
        );
        $type['unique'] = !empty($type['unique']);
        $type['palette_group'] = isset($groups[$type['palette_group']]) ? $type['palette_group'] : 'default';

        // All fields must support weight.
        if (!in_array('weight', $type['properties'])) {
          $type['properties'][] = 'weight';
        }

        // Update the default elements with some defaults.
        // Note that if a field is not removable, it doesn't have a default.
        $type['default'] += array('#form_builder' => array());
        if ($type['addable']) {
          $type['default']['#form_builder'] += array('element_type' => $key);
          if ($type['unique']) {
            $type['default']['#form_builder']['element_id'] = $key;
          }
        }
      }
      // Sort fields by weight and title.
      uasort($element_types, '_form_builder_sort');
    }
    drupal_alter('form_builder_types', $element_types);
    return $form_types;
  }

  public function getElementTypeInfo($form_type = NULL) {
    return $form_type ? $this->elementTypeInfo[$form_type] : $this->elementTypeInfo;
  }

  public function getPaletteGroupInfo($form_type, $reset = FALSE) {
    if (!isset($this->paletteGroupInfo[$form_type]) || $reset) {
      $this->paletteGroupInfo[$form_type] = module_invoke_all('form_builder_palette_groups', $form_type);
    }
    return $this->paletteGroupInfo[$form_type];
  }

  public function getPropertyInfo($form_type, $reset = FALSE) {
    if (!isset($this->propertyInfo[$form_type]) || $reset) {
      // Don't use module_invoke_all here as it uses array_merge_recursive()
      // which creates sub-arrays for duplicate array keys.
      $properties = array();
      foreach (module_implements('form_builder_properties') as $module) {
        $new_properties = module_invoke($module, 'form_builder_properties', $form_type);
        $properties += $new_properties;
        foreach ($new_properties as $k => $v) {
          $properties[$k] = array_merge($properties[$k], $new_properties[$k]);
        }
      }
      drupal_alter('form_builder_properties', $properties, $form_type);
      $defaults['class'] = $this->formTypeInfo[$form_type]['property class'];
      foreach ($properties as $property => &$params) {
        $params += $defaults;
      }
      $this->propertyInfo[$form_type] = $properties;
    }

    return $this->propertyInfo[$form_type];
  }

  /**
   * Get a form type object.
   */
  public function getForm($form_type, $form = array()) {
    if (!isset($this->formTypes[$form_type])) {
      if (!isset($this->formTypeInfo[$form_type])) {
        return FALSE;
      }
      $info = $this->formTypeInfo[$form_type];
      $class = $info['class'];
      $this->formTypes[$form_type] = new $class($form_type, $info, $form, $this);
    }
    return $this->formTypes[$form_type];
  }

  /**
   * Get element instance.
   */
  public function getElement($form_type, $element_type, $form, &$element) {
    $info = $this->elementTypeInfo[$form_type][$element_type];
    $class = $info['class'];
    return new $class($form, $info, $element, $this);
  }
}


interface FormBuilderFormInterface {

  /**
   * Construct a new instance of this form type class..
   *
   * @param string $form_type
   *   Name of the form_type.
   * @param array $params
   *   Additional parameters passed to hook_form_builder_properties().
   */
  public function __construct($form_type, $params, $form, $loader);

  /**
   * Get a specific element from the form.
   *
   * @param string $elment_id
   *   Unique ID of the element.
   *
   * @return FormBuilderElementInterface
   *   Object representing the form element.
   */
  public function getElement($element_id);

}

class FormBuilderFormBase {

  protected $formType;
  protected $params;
  protected $properties;
  protected $form;
  protected $loader;
  protected $elementArrays = array();

  /**
   * Shortcut for loading a form-object from the cache.
   */
  public static function fromCache($form_type, $form_id, $sid = NULL) {
    $form = form_builder_cache_load($form_type, $form_id, $sid = NULL);
    if ($form !== FALSE) {
      return FormBuilderLoader::instance()->getForm($form_type, $form);
    }
    return FALSE;
  }

  /**
   * {@inheritdoc}
   */
  public function __construct($form_type, $params, $form, $loader) {
    $this->formType = $form_type;
    $this->params = $params;
    $this->properties = NULL;
    $this->form = &$form;
    $this->loader = $loader;

    $q = array(&$form);
    while ($q) {
      $element = &array_pop($q);
      foreach (element_children($element) as $key) {
        $q[] = &$element[$key];
        if (isset($element[$key]['#form_builder']['element_id'])) {
          $this->elementArrays[$element[$key]['#form_builder']['element_id']] = &$element[$key];
        }
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function getElement($element_id) {
    $element = &$this->elementArrays[$element_id];
    return $this->loader
      ->getElement($this->formType, $element['#form_builder']['element_type'], $this, $element);
  }

  /**
   * {@inheritdoc}
   */
  public function getProperties($reset = FALSE) {
    if (!$this->properties || $reset) {
      $properties = $this->loader->getPropertyInfo($this->formType, $reset);
      foreach ($properties as $property => $params) {
        $class = $params['class'];
        $this->properties[$property] = new $class($property, $params, $this->formType);
      }
    }

    return $this->properties;
  }

}


interface FormBuilderElementInterface {
  public function __construct($form_type, $params, &$element, $loader);

  /**
   * (Re-)Render an element.
   *
   * @return array
   *   New FAPI array reflecting all the changes made prior to callig this
   *   method.
   */
  public function render();

  /**
   * Get a list of properties available for this element.
   *
   * @return
   *   An associative array of properties keyed by the property name.
   *   Property classes must implementing FormBuilderPropertyInterface.
   */
  public function getProperties();

  /**
   * Get a list of properties that are supported in any way by this element.
   *
   * This returns a list of all supported properties within an element, even
   * if some of those properties do not have an interface for editing or are
   * only used internally by the module providing the form type this element
   * is being saved in.
   *
   * @return
   *   A non-indexed list of properties that may be saved for this element.
   **/
  public function getSaveableProperties();

  /**
   * Get the configuration form for this element.
   */
  public function configurationForm($form, &$form_state);

  /**
   * Submit handler for the configuration form.
   */
  public function configurationSubmit(&$form, &$form_state);
}


class FormBuilderElementBase implements FormBuilderElementInterface {
  protected $form;
  protected $params;
  protected $element;
  protected $loader;
  public function __construct($form, $params, &$element, $loader) {
    $this->form = $form;
    $this->params = $params;
    $this->element = &$element;
    $this->loader = $loader;
  }

  /**
   * {@inheritdoc}
   */
  public function render() {
    return $this->element;
  }

  /**
   * {@inheritdoc}
   */
  public function getProperties() {
    $return = array();
    $properties = $this->form->getProperties();
    // Order of the properties is important because of a form-API bug.
    // See: https://www.drupal.org/node/990218.
    foreach ($this->params['properties'] as $name) {
      if (isset($properties[$name])) {
        $return[$name] = $properties[$name];
      }
    }
    return $return;
  }

  /**
   * Set the value of a property.
   *
   * This method must update the $element for rendering as well as for
   * later storage.
   *
   * @param string $property
   *   Key of the property.
   * @param mixed $value
   *   New value for the property.
   */
  protected function setProperty($property, $value) {
    // Remove empty properties entirely.
    if ($value === '' || is_null($value)) {
      unset($this->element['#'. $property]);
    }
    else {
      $this->element['#'. $property] = $value;
    }
  }

  public function getSaveableProperties() {
    return $this->params['properties'];
  }

  /**
   * {@inheritdoc}
   */
  public function configurationForm($form, &$form_state) {
    foreach ($this->getProperties() as $property) {
      $form = array_merge($form, $property->form($form_state, $this->element));
    }
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function configurationSubmit(&$form, &$form_state) {
    // Allow each property to do any necessary submission handling.
    foreach ($this->getProperties() as $property) {
      $property->submit($form, $form_state);
    }

    // Update the field according to the settings in $form_state['values'].
    $saveable = $this->getSaveableProperties();
    foreach ($form_state['values'] as $property => $value) {
      if (in_array($property, $saveable)) {
        $this->setProperty($property, $value);
      }
    }
  }
}


interface FormBuilderPropertyInterface {

  /**
   * Construct a new instance of this property class.
   *
   * @param string $property
   *   Name of the property to be manipulated by this object.
   * @param array $params
   *   Additional parameters passed to hook_form_builder_properties().
   */
  public function __construct($property, $params, $form_type_name);

  /**
   * Generate form-API elements for editing this property.
   *
   * @param array $form_state
   *   Form API form_state of the field configure form.
   * @param array $element
   *   Form-API array representing the currently stored value of all properties
   *   on this form $element. Use this to get the "current" values.
   *
   * @return array
   *   Form-API array that will be merged into the field configure form.
   */
  public function form(&$form_state, $element);

  /**
   * Submit handler for the editing form().
   *
   * This function is responsible to store the new value into the $form_state.
   * The value must be located at $form_state['values'][$property].
   *
   * @param array $form_state
   *   Form API form_state of the field configure form.
   */
  public function submit($form, &$form_state);

}


class FormBuilderPropertyBase implements FormBuilderPropertyInterface {

  protected $property;
  protected $params;
  protected $form_type_name;

  /**
   * {@inheritdoc}
   */
  public function __construct($property, $params, $form_type_name) {
    $this->property = $property;
    $this->params = $params;
    $this->formTypeName = $form_type_name;
  }

  /**
   * {@inheritdoc}
   */
  public function form(&$form_state, $element) {
    if (isset($this->params['form']) && function_exists($this->params['form'])) {
      $function = $this->params['form'];
      $p = $this->property;
      // Set a default value on the property to avoid notices.
      $element['#' . $p] = isset($element['#' . $p]) ? $element['#' . $p] : NULL;
      return $function($form_state, $this->formTypeName, $element, $p);
    }
    return array();
  }

  /**
   * {@inheritdoc}
   */
  public function submit($form, &$form_state) {
    if (isset($this->params['submit'])) {
      foreach ($this->params['submit'] as $function) {
        if (function_exists($function)) {
          $function($form, $form_state);
        }
      }
    }
  }

}
