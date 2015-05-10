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
      'class' => 'FormBuilderFormTypeBase',
      'property class' => 'FormBuilderPropertyBase',
    );

    $form_types = module_invoke_all('form_builder_form_types');
    $element_types = $this->invokeElementTypeInfo();
    foreach (array_keys($element_types) as $form_type) {
      $form_types += array($form_type => array());
    }

    foreach ($form_types as $form_type => &$info) {
      $info += $defaults;
      $element_types += array($form_type => array());
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
  public function getFormType($form_type) {
    if (!isset($this->formTypes[$form_type])) {
      if (!isset($this->formTypeInfo[$form_type])) {
        return FALSE;
      }
      $info = $this->formTypeInfo[$form_type];
      $class = $info['class'];
      $this->formTypes[$form_type] = new $class($form_type, $info, $this);
    }
    return $this->formTypes[$form_type];
  }
}


interface FormBuilderFormTypeInterface {

  /**
   * Construct a new instance of this form type class..
   *
   * @param string $form_type
   *   Name of the form_type.
   * @param array $params
   *   Additional parameters passed to hook_form_builder_properties().
   */
  public function __construct($form_type, $params, $loader);

  /**
   * Get a list of properties available for this form type.
   */
  public function getProperties($reset = FALSE);

  /**
   * Get a list of all element types for this form type.
   */
  public function getElementTypes();

  /**
   * Get a list of palette groups.
   */
  public function getPaletteGroups();

  /**
   * Get a list of properties available for an element type.
   *
   * @param $element_type
   *   Form builder element-type (ie. FAPI #type).
   * @return
   *   An associative array of properties supported for editing, keyed by
   *   the property name. All properties must be instances of a class
   *   implementing FormBuilderPropertyInterface.
   */
  public function getElementTypeProperties($element_type, $reset = FALSE);

  /**
   * Get a list of properties that are supported in any way by an element.
   *
   * This returns a list of all supported properties within an element, even
   * if some of those properties do not have an interface for editing or are
   * only used internally by the module providing the form type this element
   * is being saved in.
   *
   * @param $element_type
   *   The element.
   * @return
   *   A non-indexed list of properties that may be saved for this element.
   **/
  public function getElementSaveableProperties($element_type);

  /**
   * Store a properties value into the element-array.
   *
   * This method must update the $element for rendering as well as for
   * later storage.
   *
   * @param array $element
   *   FAPI array representing the element.
   * @param string $property
   *   Key of the property.
   * @param mixed $value
   *   New value for the property.
   */
  public function storeProperty(&$element, $property, $value);

  /**
   * (Re-)Render an element.
   *
   * @param array $element
   *   FAPI array representing the element.
   * @return array
   *   New FAPI array reflecting all the changes made prior to callig this
   *   method.
   */
  public function renderElement($element);

}

class FormBuilderFormTypeBase {

  protected $formType;
  protected $params;
  protected $properties;
  protected $loader;

  /**
   * {@inheritdoc}
   */
  public function __construct($form_type, $params, $loader) {
    $this->formType = $form_type;
    $this->params = $params;
    $this->properties = NULL;
    $this->loader = $loader;
  }

  /**
   * {@inheritdoc}
   */
  public function getProperties($reset = FALSE) {
    if (!$this->properties || $reset) {
      $properties = $this->loader->getPropertyInfo($this->formType, $reset);
      foreach ($properties as $property => $params) {
        $class = $params['class'];
        $this->properties[$property] = new $class($property, $params);
      }
    }

    return $this->properties;
  }

  /**
   * {@inheritdoc}
   */
  public function getElementTypes() {
    return $this->loader->getElementTypeInfo($this->formType);
  }

  /**
   * {@inheritdoc}
   */
  public function getPaletteGroups() {
    return $this->loader->getPaletteGroupInfo($this->formType);
  }

  /**
   * {@inheritdoc}
   */
  public function getElementTypeProperties($element_type, $reset = FALSE) {
    // Get the list of all properties for this type.
    $properties = $this->getProperties($reset);
    // Get the list of supported properties per field in this form type.
    $form_type = $this->getElementTypes();

    $element_properties = array();
    if (isset($form_type[$element_type]['properties'])) {
      foreach ($form_type[$element_type]['properties'] as $property) {
        if (isset($properties[$property])) {
          $element_properties[$property] = $properties[$property];
        }
      }
    }

    return $element_properties;
  }

  /**
   * {@inheritdoc}
   */
  public function getElementSaveableProperties($element_type) {
    $types = $this->getElementTypes();
    return $types[$element_type]['properties'];
  }

  /**
   * {@inheritdoc}
   */
  public function storeProperty(&$element, $property, $value) {
      // Remove empty properties entirely.
      if ($value === '' || is_null($value)) {
        unset($element['#'. $property]);
      }
      else {
        $element['#'. $property] = $value;
      }
  }

  /**
   * {@inheritdoc}
   */
  public function renderElement($element) {
    return $element;
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
  public function __construct($poperty, $params);

  /**
   * Generate form-API elements for editing this property.
   *
   * @param array $form_state
   *   Form API form_state of the field configure form.
   * @param string $form_type
   *   Name of the form_type.
   * @param array $element
   *   Form-API array representing the currently stored value of all properties
   *   on this form $element. Use this to set form-elements' default values.
   *
   * @return array
   *   Form-API array that will be merged into the field configure form.
   */
  public function form(&$form_state, $form_type, $element);

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

  /**
   * {@inheritdoc}
   */
  public function __construct($property, $params) {
    $this->property = $property;
    $this->params = $params;
  }

  /**
   * {@inheritdoc}
   */
  public function form(&$form_state, $form_type, $element) {
    if (isset($this->params['form']) && function_exists($this->params['form'])) {
      $function = $this->params['form'];
      $p = $this->property;
      // Set a default value on the property to avoid notices.
      $element['#' . $p] = isset($element['#' . $p]) ? $element['#' . $p] : NULL;
      return $function($form_state, $form_type, $element, $p);
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
